#ifndef IMAGE_ALIGNMENT_PROJECT_H
#define IMAGE_ALIGNMENT_PROJECT_H

#include "ImageAlignmentContainer.h"

#include <opencv2/opencv.hpp>
#include <opencv2/reg/mappergradproj.hpp>
#include <opencv2/reg/mapprojec.hpp>
#include <opencv2/reg/mapper.hpp>

#include <vector>

class ImageAlignmentProject : public cv::reg::MapperGradProj
{

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  public:
    typedef class ImageAlignmentContainer Container;
    virtual cv::Ptr<cv::reg::Map> calculate(const cv::Mat &img1, const Container &container,
                                            cv::Ptr<cv::reg::Map> init = cv::Ptr<cv::reg::Map>()) const;
};

#endif // IMAGE_ALIGNMENT_PROJECT_H