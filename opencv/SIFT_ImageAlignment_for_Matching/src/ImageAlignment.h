#ifndef IMAGE_ALIGNMENT_H
#define IMAGE_ALIGNMENT_H

#include "ImageAlignmentProject.h"
#include "ImageAlignmentPyramid.h"

#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/reg/mapper.hpp>
#include <opencv2/reg/mappergradproj.hpp>
#include <opencv2/reg/mapperpyramid.hpp>

#include <iostream>
#include <vector>

class ImageAlignment
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*constructors and destructors*/
  public:
    /**
     * @brief: constructor
     * @parameter imageObject: templat image 
     */
    ImageAlignment(const cv::Mat &imageObject, const int numLev = 3, const int numIterPerScale = 5);
    virtual ~ImageAlignment();

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*protected method*/
  protected:
    /**
     * @brief: find Homography matrix from 'imageScene' to 'm_imageObject' roughly 
     *          through feature-based algorithm, using SIFT for feature detection
     */
    bool _getHomography(const cv::Mat &imageScene);

    /**
     * @brief: get imageScene's roi 
     */
    bool _getRoiByHomography(const cv::Mat imageScene, cv::Mat &imageSceneRoi);

    /**
     * @brief: find transform matrix from 'image' to 'm_imageObject'
     */
    bool _pixelBasedAlignment(const cv::Mat &image);

  public:
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*public methods*/
    const int numLev() const
    {
        return m_numLev;
    }

    const int numIterPerScale() const
    {
        return m_numIterPerScale;
    }

    void setNumIterPerScale(const int numIterPerScale)
    {
        m_numIterPerScale = numIterPerScale;
        m_mapperPyramid->numIterPerScale_ = m_numIterPerScale;
    }

    void setNumLev(const int numLev)
    {
        m_numLev = numLev;
        m_mapperPyramid->numLev_ = numLev;
    }

    cv::Mat matrix() const
    {
        return m_matrixOfImage2Object;
    }

    cv::Mat inverseMatrix() const
    {
        return m_matrixOfImage2Object.inv();
    }

    cv::Mat H() const
    {
        return m_H;
    }

    cv::Mat M() const
    {
        return m_M;
    }

    bool calculateMatrixOfImage2Object(const cv::Mat &image, cv::Mat &matrixOfImage2Object);
    cv::Mat calculateMatrixOfImage2Object(const cv::Mat &image);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*private members*/
  private:
    cv::Mat m_imageObject;
    cv::Mat m_imageObjectF;
    cv::Mat m_H; // Homography matrix find by featires
    cv::Mat m_M; // Projective matrix find by pixel alignment
    cv::Mat m_matrixOfImage2Object;
    cv::Ptr<cv::xfeatures2d::SIFT> m_siftDetector; // sift detector
    // cv::Ptr<cv::ORB> m_orbDetector;                // ORB detector
    cv::Ptr<cv::DescriptorMatcher> m_flannMatcher; // matching descriptor vectors with a FLANN based matcher
    // cv::Ptr<cv::DescriptorMatcher> m_bfMatcher;
    cv::Ptr<cv::reg::MapperGradProj> m_mapper;
    cv::reg::MapperPyramid *m_mapperPyramid;

    std::vector<cv::KeyPoint> m_keypointsObject; // object's keypoints
    std::vector<cv::KeyPoint> m_keypointsScene;  // scene's keypoints

    cv::Mat m_descriptorsObject; // object's desciptors
    cv::Mat m_descriptorsScene;  // scene's desciptors

    int m_numLev;          // number of pyramid level
    int m_numIterPerScale; // iteration times of per level in pyramid

    class ImageAlignmentPyramid *m_offlineProjectCalculator;
    class ImageAlignmentProject *m_baseMapper;

    const float RATIO_THRESH = 1.0 / 2.0;
    const size_t HOMOGRAPHY_MIN_ALLOWED_POINT_NUMBER = 4;
};

#endif // IMAGE_ALIGNMENT_H