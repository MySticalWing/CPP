#include "ImageAlignmentProject.h"
#include "ImageAlignmentContainer.h"

#include <opencv2/opencv.hpp>
#include <opencv2/reg/mappergradproj.hpp>
#include <opencv2/reg/map.hpp>

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////
cv::Ptr<cv::reg::Map> ImageAlignmentProject::calculate(const cv::Mat &image1, const Container &container,
                                                       cv::Ptr<cv::reg::Map> init) const
{
    assert(image1.size() == container.m_image.size());

    const cv::Mat &image2 = container.m_image;
    cv::Mat img2, img1;

    image1.convertTo(img1, CV_64F);

    if (!init.empty())
    {
        // We have initial values for the registration: we move img2 to that initial reference
        init->inverseWarp(image2, img2);
    }
    else
    {
        image2.convertTo(img2, CV_64F);
    }

    cv::Mat imgDiff = img2 - img1;
    cv::Vec<double, 8> b;

    // Calculation of b
    b(0) = -cv::sum(cv::sum(imgDiff.mul(container.m_xIx)))[0];
    b(1) = -cv::sum(cv::sum(imgDiff.mul(container.m_yIx)))[0];
    b(2) = -cv::sum(cv::sum(imgDiff.mul(container.m_grad_x)))[0];
    b(3) = -cv::sum(cv::sum(imgDiff.mul(container.m_xIy)))[0];
    b(4) = -cv::sum(cv::sum(imgDiff.mul(container.m_yIy)))[0];
    b(5) = -cv::sum(cv::sum(imgDiff.mul(container.m_grad_y)))[0];
    b(6) = cv::sum(cv::sum(imgDiff.mul(container.m_xG)))[0];
    b(7) = cv::sum(cv::sum(imgDiff.mul(container.m_yG)))[0];

    // Calculate affine transformation. We use Cholesky decomposition, as A is symmetric.
    cv::Vec<double, 8> k = container.m_invA * b;

    cv::Matx<double, 3, 3> H(k(0) + 1., k(1), k(2), k(3), k(4) + 1., k(5), k(6), k(7), 1.);
    if (init.empty())
    {
        return cv::Ptr<cv::reg::Map>(new cv::reg::MapProjec(H));
    }
    else
    {
        cv::Ptr<cv::reg::MapProjec> newTr(new cv::reg::MapProjec(H));
        cv::reg::MapProjec *initPtr = dynamic_cast<cv::reg::MapProjec *>(init.get());
        cv::Ptr<cv::reg::MapProjec> oldTr(new cv::reg::MapProjec(initPtr->getProjTr()));
        oldTr->compose(newTr);
        return oldTr;
    }
}