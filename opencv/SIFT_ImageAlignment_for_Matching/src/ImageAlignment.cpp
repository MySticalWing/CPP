#include "ImageAlignment.h"
#include "ImageAlignmentPyramid.h"

#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/reg/mapper.hpp>
#include <opencv2/reg/mappergradproj.hpp>
#include <opencv2/reg/mapperpyramid.hpp>
#include <opencv2/reg/map.hpp>
#include <opencv2/reg/mapprojec.hpp>

#include <opencv2/line_descriptor.hpp>

#include <iostream>
#include <vector>
#include <assert.h>

#define MEASURE_TIME false
#define DEBUG false

#define SIFT_TEST false

//////////////////////////////////////////////////////////////////////////////////////////////////////////
ImageAlignment::ImageAlignment(const cv::Mat &imageObject, const int numLev, const int numIterPerScale)
    : m_numLev(numLev), m_numIterPerScale(numIterPerScale)
{
    assert(!imageObject.empty());

    imageObject.copyTo(m_imageObject);
    m_imageObject.convertTo(m_imageObjectF, CV_64F);

    // init feature detector and template's keypoints and descriptors
    m_siftDetector = cv::xfeatures2d::SIFT::create();
    m_siftDetector->detectAndCompute(m_imageObject, cv::noArray(),
                                     m_keypointsObject, m_descriptorsObject);

    // m_orbDetector = cv::ORB::create();
    // m_orbDetector->detectAndCompute(m_imageObject, cv::noArray(),
    // m_keypointsObject, m_descriptorsObject);

    // init feature matcher
    m_flannMatcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
    // m_bfMatcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE_HAMMING);

    // init mapper
    m_mapper = cv::makePtr<cv::reg::MapperGradProj>();
    m_mapperPyramid = new cv::reg::MapperPyramid(m_mapper);
    m_mapperPyramid->numIterPerScale_ = numIterPerScale;
    m_mapperPyramid->numLev_ = numLev;

    m_baseMapper = new ImageAlignmentProject();
    m_offlineProjectCalculator = new ImageAlignmentPyramid(*m_baseMapper, numLev, numIterPerScale);
    m_offlineProjectCalculator->setTemplate(m_imageObjectF);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
ImageAlignment::~ImageAlignment()
{
    delete m_mapperPyramid;
    delete m_baseMapper;
    delete m_offlineProjectCalculator;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ImageAlignment::_getHomography(const cv::Mat &imageScene)
{
#if MEASURE_TIME
    int64 start, end;
#endif

    assert(!imageScene.empty());

#if MEASURE_TIME
    start = cv::getTickCount();
#endif

    m_keypointsScene.clear();
    // detect input scene's keypints and descriptors
    m_siftDetector->detectAndCompute(imageScene, cv::noArray(), m_keypointsScene, m_descriptorsScene, false);
    // m_orbDetector->detectAndCompute(imageScene, cv::noArray(), m_keypointsScene, m_descriptorsScene, false);

    static const int KNN_SIZE = 2;

    if (m_keypointsScene.size() <= KNN_SIZE)
    {
        m_H = cv::Mat::eye(3, 3, CV_64F);
        return false;
    }

#if MEASURE_TIME
    end = cv::getTickCount();
    std::cout << "detectAndCompute() costs:" << (end - start) / cv::getTickFrequency() * 1000 << " ms" << std::endl;
#endif

    // matching descriptor vectors with a FLANN based matcher
    std::vector<std::vector<cv::DMatch>> knnMatches;
    m_flannMatcher->knnMatch(m_descriptorsObject, m_descriptorsScene, knnMatches, KNN_SIZE, true);
    // std::vector<cv::DMatch> matches;
    // m_flannMatcher->match(m_descriptorsObject, m_descriptorsScene, matches);

    // filter matches using the ratio test
    std::vector<cv::DMatch> goodMatches;
    for (size_t i = 0; i < knnMatches.size(); i++)
    {
        cv::DMatch &bestMatch = knnMatches[i][0];
        cv::DMatch &betterMatch = knnMatches[i][1];
        float ratio = bestMatch.distance / betterMatch.distance;
        if (ratio < RATIO_THRESH)
        {
            goodMatches.push_back(bestMatch);
        }
    }

#if SIFT_TEST
    cv::Mat showImage;
    cv::drawMatches(m_imageObject, m_keypointsObject, imageScene, m_keypointsScene, goodMatches, showImage,
                    cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(),
                    cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
    cv::imshow("Matches", showImage);
#endif

    // std::cout << goodMatches.size() << std::endl;
    if (goodMatches.size() < HOMOGRAPHY_MIN_ALLOWED_POINT_NUMBER)
    {
        m_H = cv::Mat::eye(3, 3, CV_64F);
        return false;
    }

    // find homography matrix from object to scene
    size_t sz = goodMatches.size();
    std::vector<cv::Point2f> obj(sz), scene(sz);
    for (size_t i = 0; i < sz; i++)
    {
        obj[i] = m_keypointsObject[goodMatches[i].queryIdx].pt;
        scene[i] = m_keypointsScene[goodMatches[i].trainIdx].pt;
    }

    m_H = cv::findHomography(scene, obj, cv::RANSAC);

    if (m_H.empty())
    {
        m_H = cv::Mat::eye(3, 3, CV_64F);
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ImageAlignment::_getRoiByHomography(const cv::Mat imageScene, cv::Mat &imageSceneRoi)
{
    assert(!imageScene.empty());

    int w = m_imageObject.cols;
    int h = m_imageObject.rows;

    cv::Mat warppedScene;
    cv::warpPerspective(imageScene, warppedScene, m_H, imageScene.size());

    w = (w < warppedScene.cols) ? w : warppedScene.cols;
    h = (h < warppedScene.rows) ? h : warppedScene.rows;

    imageSceneRoi = cv::Mat(warppedScene, cv::Rect(0, 0, w, h));

    if ((w != m_imageObject.cols) || (h != m_imageObject.rows))
    {
        cv::resize(imageSceneRoi, imageSceneRoi, m_imageObject.size());
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ImageAlignment::_pixelBasedAlignment(const cv::Mat &image)
{
#if MEASURE_TIME
    int64 start, end;
#endif

    assert(!image.empty());

    cv::Mat imageF;
    if ((image.depth() != CV_64F) || (image.depth() != CV_32F))
    {
        image.convertTo(imageF, CV_64F);
    }
    else
    {
        imageF = image;
    }

    cv::Ptr<cv::reg::Map> mapPtr;

#if MEASURE_TIME
    start = cv::getTickCount();
#endif

    // Find homography matrix from image to m_imageObject
    mapPtr = m_mapperPyramid->calculate(imageF, m_imageObjectF);

#if MEASURE_TIME
    end = cv::getTickCount();
    std::cout << "calculate() costs:" << (end - start) / cv::getTickFrequency() * 1000 << " ms" << std::endl;
#endif

    // Extract homography matrix
    cv::reg::MapProjec *mapProjective = dynamic_cast<cv::reg::MapProjec *>(mapPtr.get());
    mapProjective->normalize();

    m_M = cv::Mat(mapProjective->getProjTr());

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ImageAlignment::calculateMatrixOfImage2Object(const cv::Mat &image, cv::Mat &matrixOfImage2Object)
{

    _getHomography(image);

    cv::Mat imageRoi;
    _getRoiByHomography(image, imageRoi);

    _pixelBasedAlignment(imageRoi);

    m_matrixOfImage2Object = m_M * m_H;
    matrixOfImage2Object = m_matrixOfImage2Object;

#if DEBUG
    std::cout << "M:" << std::endl
              << m_M << std::endl;
    std::cout << "H:" << std::endl
              << m_H << std::endl;
    std::cout << "m:" << std::endl
              << matrixOfImage2Object << std::endl;
#endif

    return true;
}

cv::Mat ImageAlignment::calculateMatrixOfImage2Object(const cv::Mat &image)
{
#if MEASURE_TIME
    int64 start, end;
    start = cv::getTickCount();
#endif

    _getHomography(image);

#if MEASURE_TIME
    end = cv::getTickCount();
    std::cout << "GetHomography() costs: " << (end - start) / cv::getTickFrequency() * 1000 << " ms" << std::endl;
#endif

    cv::Mat imageRoi;
    _getRoiByHomography(image, imageRoi);

#if MEASURE_TIME
    start = cv::getTickCount();
#endif

    // get M
    cv::Ptr<cv::reg::Map> mapPtr = m_offlineProjectCalculator->calculate(imageRoi);

#if MEASURE_TIME
    end = cv::getTickCount();
    std::cout << "m_offlineProjectCalculator->calculate() costs: " << (end - start) / cv::getTickFrequency() * 1000 << " ms" << std::endl;
#endif

    // Extract m_M
    cv::reg::MapProjec *mapProjective = dynamic_cast<cv::reg::MapProjec *>(mapPtr.get());
    mapProjective->normalize();
    m_M = cv::Mat(mapProjective->getProjTr());

    return (m_M * m_H);
}