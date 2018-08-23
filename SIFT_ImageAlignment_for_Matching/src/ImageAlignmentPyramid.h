#ifndef IMAGE_ALIGNMENT_PYRAMID_H
#define IMAGE_ALIGNMENT_PYRAMID_H

#include "ImageAlignmentContainer.h"
#include "ImageAlignmentProject.h"

#include <opencv2/reg/map.hpp>
#include <opencv2/reg/mapper.hpp>
#include <opencv2/reg/mapperpyramid.hpp>

#include <vector>

class ImageAlignmentPyramid //: public cv::reg::MapperPyramid
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
  public:
    ImageAlignmentPyramid(const ImageAlignmentProject &baseMapper, const int numLev = 3,
                          const int numIterPerScale = 5);

    virtual ~ImageAlignmentPyramid();

    cv::Ptr<cv::reg::Map> calculate(const cv::Mat imageScene);

    void setTemplate(const cv::Mat &imageTemplate);

    void setNumLev(const int numLev)
    {
        if (numLev <= 0)
        {
            return;
        }
        m_numLev = numLev;
        setTemplate(m_imageObjectF);
    }

    void setNumIterPerScale(const int numIterPerScale)
    {
        if (numIterPerScale <= 0)
        {
            return;
        }
        m_numIterPerScale = numIterPerScale;
    }

    int numLev() const
    {
        return m_numLev;
    }
    int numIterPerScale() const
    {
        return m_numIterPerScale;
    }

    void showPyramid(const std::vector<cv::Mat> &pyramid, const std::string &windowName);

  protected:
    void _generatePyramidObject()
    {
        _generateImagePyramid(m_imageObjectF, m_pyramidObjectF, m_numLev);
    }

    void _generateImagePyramid(const cv::Mat &image, std::vector<cv::Mat> &pyramid, const int numLev);

    ////////////////////////////////////////////////////////////////////////////////////////////////
  private:
    std::vector<class ImageAlignmentContainer> m_values;
    std::vector<cv::Mat> m_pyramidObjectF;
    std::vector<cv::Mat> m_pyramidSceneF;

    cv::Mat m_imageObjectF;
    const ImageAlignmentProject &m_baseMapper;

    int m_numLev;
    int m_numIterPerScale;
};

#endif //IMAGE_ALIGNMENT_PYRAMID_H