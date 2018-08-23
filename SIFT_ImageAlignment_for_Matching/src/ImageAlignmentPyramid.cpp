#include "ImageAlignmentPyramid.h"
#include "ImageAlignmentContainer.h"
#include "ImageAlignmentProject.h"

#include <opencv2/reg/mapper.hpp>
#include <opencv2/reg/mapperpyramid.hpp>

#include <assert.h>
#include <vector>

#define DEBUG false
#define MEASURE_TIME false

#if DEBUG

#include <sstream>
#include <iostream>

#endif

using std::cin;
using std::cout;
using std::endl;

////////////////////////////////////////////////////////////////////////////////////////////////
ImageAlignmentPyramid::ImageAlignmentPyramid(const ImageAlignmentProject &baseMapper,
                                             const int numLev, const int numIterPerScale)
    : m_baseMapper(baseMapper), m_numLev(numLev), m_numIterPerScale(numIterPerScale)
{
    ;
}

////////////////////////////////////////////////////////////////////////////////////////////////
ImageAlignmentPyramid::~ImageAlignmentPyramid()
{
    ;
}

////////////////////////////////////////////////////////////////////////////////////////////////
cv::Ptr<cv::reg::Map> ImageAlignmentPyramid::calculate(const cv::Mat imageScene)
{
#if MEASURE_TIME
    int64 start, end;
#endif

    assert(!m_imageObjectF.empty());
    assert(imageScene.size() == m_imageObjectF.size());

    // generate scene image's pyramid
    // std::vector<cv::Mat> pyramidScene;
    _generateImagePyramid(imageScene, m_pyramidSceneF, m_numLev);

#if DEBUG
    showPyramid(m_pyramidObjectF, "Object");
    showPyramid(pyramidScene, "Scene");
#endif

    cv::Ptr<cv::reg::Map> ident = m_baseMapper.getMap();

#if MEASURE_TIME
    start = cv::getTickCount();
#endif

    for (int lv_i = 0; lv_i < m_numLev; ++lv_i)
    {

        cv::Mat &currScene = m_pyramidSceneF[m_numLev - 1 - lv_i];
        ImageAlignmentContainer &currValues = m_values[m_numLev - 1 - lv_i];

#if DEBUG
        std::stringstream ss;
        ss << lv_i;
        std::string num;
        ss >> num;

        cv::Mat scene, container;
        currValues.m_image.convertTo(container, CV_8U);
        cv::imshow("Curr Scene " + num, currScene);
        cv::imshow("Curr Container " + num, container);

        return m_baseMapper.getMap();
#endif

        //Scale the transformation as we are incresing the resolution in each iteration if (lv_i != 0)
        {
            ident->scale(2.);
        }
        for (int it_i = 0; it_i < m_numIterPerScale; ++it_i)
        {

            ident = m_baseMapper.calculate(currScene, currValues, ident);
        }
    }

#if MEASURE_TIME
    end = cv::getTickCount();
    cout << "m_baseMapper.calculate() Cost: " << (end - start) / cv::getTickFrequency() * 1000 << " ms" << endl;
#endif

    return ident;
}

////////////////////////////////////////////////////////////////////////////////////////////////
void ImageAlignmentPyramid::setTemplate(const cv::Mat &imageTemplate)
{
    if (imageTemplate.depth() == CV_64F || imageTemplate.depth() == CV_32F)
    {
        imageTemplate.copyTo(m_imageObjectF);
    }
    else
    {
        imageTemplate.convertTo(m_imageObjectF, CV_64F);
    }

    // generate object image pyramid
    _generatePyramidObject();

    m_pyramidSceneF = std::vector<cv::Mat>(4);
    _generateImagePyramid(m_imageObjectF, m_pyramidSceneF, m_numLev);

    // calculate each level's A and other value
    // m_values = std::vector<ImageAlignmentContainer>(m_numLev);
    for (int i = 0; i < m_numLev; i++)
    {
        m_values.push_back(ImageAlignmentContainer(m_pyramidObjectF[i]));
        // m_values[i] = ImageAlignmentContainer(m_pyramidObjectF[i]);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void ImageAlignmentPyramid::_generateImagePyramid(const cv::Mat &image, std::vector<cv::Mat> &pyramid,
                                                  const int numLev)
{
    pyramid = std::vector<cv::Mat>(numLev);
    pyramid[0] = image;
    for (int i = 1; i < numLev; i++)
    {
        cv::pyrDown(pyramid[i - 1], pyramid[i]);
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
void ImageAlignmentPyramid::showPyramid(const std::vector<cv::Mat> &pyramid, const std::string &windowName)
{
    cv::Mat object;
    // show object pyramid
    for (int i = 0; i < m_numLev; i++)
    {
        std::stringstream ss;
        ss << i;
        std::string num;
        ss >> num;
        pyramid[i].convertTo(object, CV_8U);
        // cout << object.size() << endl;
        cv::imshow(windowName + num, object);
    }
}