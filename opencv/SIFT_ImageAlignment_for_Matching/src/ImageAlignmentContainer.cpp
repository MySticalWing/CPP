#include "ImageAlignmentContainer.h"

#include <opencv2/opencv.hpp>
#include <assert.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////
ImageAlignmentContainer::ImageAlignmentContainer(const cv::Mat &imageF)
{
    imageF.copyTo(m_image);
    _calculateMembers(imageF);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
ImageAlignmentContainer::~ImageAlignmentContainer()
{
    ;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void ImageAlignmentContainer::_calculateMembers(const cv::Mat &image)
{
    // Get gradient in all channels
    _gradient(image, m_grad_x, m_grad_y);

    // Matrices with reference frame coordinates
    _grid(image, m_grid_r, m_grid_c);

    // Calculate parameters using least squares

    // For each value in A, all the matrix elements are added and then the channels are also added,
    // so we have two calls to "sum". The result can be found in the first element of the final
    // Scalar object.
    m_xIx = m_grid_c.mul(m_grad_x);
    m_xIy = m_grid_c.mul(m_grad_y);
    m_yIx = m_grid_r.mul(m_grad_x);
    m_yIy = m_grid_r.mul(m_grad_y);
    cv::Mat Ix2 = m_grad_x.mul(m_grad_x);
    cv::Mat Iy2 = m_grad_y.mul(m_grad_y);
    cv::Mat xy = m_grid_c.mul(m_grid_r);
    cv::Mat IxIy = m_grad_x.mul(m_grad_y);
    cv::Mat x2 = m_grid_c.mul(m_grid_c);
    cv::Mat y2 = m_grid_r.mul(m_grid_r);
    cv::Mat G = m_xIx + m_yIy;
    cv::Mat G2 = sqr(G);
    cv::Mat IxG = m_grad_x.mul(G);
    cv::Mat IyG = m_grad_y.mul(G);
    m_xG = m_grid_c.mul(G);
    m_yG = m_grid_r.mul(G);

    m_A(0, 0) = sum(sum(x2.mul(Ix2)))[0];
    m_A(1, 0) = sum(sum(xy.mul(Ix2)))[0];
    m_A(2, 0) = sum(sum(m_grid_c.mul(Ix2)))[0];
    m_A(3, 0) = sum(sum(x2.mul(IxIy)))[0];
    m_A(4, 0) = sum(sum(xy.mul(IxIy)))[0];
    m_A(5, 0) = sum(sum(m_grid_c.mul(IxIy)))[0];
    m_A(6, 0) = -sum(sum(x2.mul(IxG)))[0];
    m_A(7, 0) = -sum(sum(xy.mul(IxG)))[0];

    m_A(1, 1) = sum(sum(y2.mul(Ix2)))[0];
    m_A(2, 1) = sum(sum(m_grid_r.mul(Ix2)))[0];
    m_A(3, 1) = m_A(4, 0);
    m_A(4, 1) = sum(sum(y2.mul(IxIy)))[0];
    m_A(5, 1) = sum(sum(m_grid_r.mul(IxIy)))[0];
    m_A(6, 1) = m_A(7, 0);
    m_A(7, 1) = -sum(sum(y2.mul(IxG)))[0];

    m_A(2, 2) = sum(sum(Ix2))[0];
    m_A(3, 2) = m_A(5, 0);
    m_A(4, 2) = m_A(5, 1);
    m_A(5, 2) = sum(sum(IxIy))[0];
    m_A(6, 2) = -sum(sum(m_grid_c.mul(IxG)))[0];
    m_A(7, 2) = -sum(sum(m_grid_r.mul(IxG)))[0];

    m_A(3, 3) = sum(sum(x2.mul(Iy2)))[0];
    m_A(4, 3) = sum(sum(xy.mul(Iy2)))[0];
    m_A(5, 3) = sum(sum(m_grid_c.mul(Iy2)))[0];
    m_A(6, 3) = -sum(sum(x2.mul(IyG)))[0];
    m_A(7, 3) = -sum(sum(xy.mul(IyG)))[0];

    m_A(4, 4) = sum(sum(y2.mul(Iy2)))[0];
    m_A(5, 4) = sum(sum(m_grid_r.mul(Iy2)))[0];
    m_A(6, 4) = m_A(7, 3);
    m_A(7, 4) = -sum(sum(y2.mul(IyG)))[0];

    m_A(5, 5) = sum(sum(Iy2))[0];
    m_A(6, 5) = -sum(sum(m_grid_c.mul(IyG)))[0];
    m_A(7, 5) = -sum(sum(m_grid_r.mul(IyG)))[0];

    m_A(6, 6) = sum(sum(x2.mul(G2)))[0];
    m_A(7, 6) = sum(sum(xy.mul(G2)))[0];

    m_A(7, 7) = sum(sum(y2.mul(G2)))[0];

    // Upper half values (A is symmetric)
    m_A(0, 1) = m_A(1, 0);
    m_A(0, 2) = m_A(2, 0);
    m_A(0, 3) = m_A(3, 0);
    m_A(0, 4) = m_A(4, 0);
    m_A(0, 5) = m_A(5, 0);
    m_A(0, 6) = m_A(6, 0);
    m_A(0, 7) = m_A(7, 0);

    m_A(1, 2) = m_A(2, 1);
    m_A(1, 3) = m_A(3, 1);
    m_A(1, 4) = m_A(4, 1);
    m_A(1, 5) = m_A(5, 1);
    m_A(1, 6) = m_A(6, 1);
    m_A(1, 7) = m_A(7, 1);

    m_A(2, 3) = m_A(3, 2);
    m_A(2, 4) = m_A(4, 2);
    m_A(2, 5) = m_A(5, 2);
    m_A(2, 6) = m_A(6, 2);
    m_A(2, 7) = m_A(7, 2);

    m_A(3, 4) = m_A(4, 3);
    m_A(3, 5) = m_A(5, 3);
    m_A(3, 6) = m_A(6, 3);
    m_A(3, 7) = m_A(7, 3);

    m_A(4, 5) = m_A(5, 4);
    m_A(4, 6) = m_A(6, 4);
    m_A(4, 7) = m_A(7, 4);

    m_A(5, 6) = m_A(6, 5);
    m_A(5, 7) = m_A(7, 5);

    m_A(6, 7) = m_A(7, 6);

    m_invA = m_A.inv(cv::DECOMP_CHOLESKY);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void ImageAlignmentContainer::_gradient(const cv::Mat &image, cv::Mat &Ix, cv::Mat &Iy)
{
    cv::Mat xkern = (cv::Mat_<double>(1, 3) << -1., 0., 1.) / 2.;
    filter2D(image, Ix, -1, xkern, cv::Point(-1, -1), 0., cv::BORDER_REPLICATE);

    cv::Mat ykern = (cv::Mat_<double>(3, 1) << -1., 0., 1.) / 2.;
    filter2D(image, Iy, -1, ykern, cv::Point(-1, -1), 0., cv::BORDER_REPLICATE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Tp>
void fillGridMatrices(const cv::Mat img, cv::Mat grid_r, cv::Mat grid_c)
{
    if (img.channels() == 1)
    {
        for (int r_i = 0; r_i < img.rows; ++r_i)
        {
            for (int c_i = 0; c_i < img.cols; ++c_i)
            {
                grid_r.at<_Tp>(r_i, c_i) = (_Tp)r_i;
                grid_c.at<_Tp>(r_i, c_i) = (_Tp)c_i;
            }
        }
    }
    else
    {
        cv::Vec<_Tp, 3> ones((_Tp)1, (_Tp)1, (_Tp)1);
        for (int r_i = 0; r_i < img.rows; ++r_i)
        {
            for (int c_i = 0; c_i < img.cols; ++c_i)
            {
                grid_r.at<cv::Vec<_Tp, 3>>(r_i, c_i) = (_Tp)r_i * ones;
                grid_c.at<cv::Vec<_Tp, 3>>(r_i, c_i) = (_Tp)c_i * ones;
            }
        }
    }
}

void ImageAlignmentContainer::_grid(const cv::Mat &img, cv::Mat &grid_r, cv::Mat &grid_c) const
{
    CV_DbgAssert(img.channels() == 1 || img.channels() == 3);

    // Matrices with reference frame coordinates
    grid_r.create(img.size(), img.type());
    grid_c.create(img.size(), img.type());

    if (img.depth() == CV_8U)
        fillGridMatrices<uchar>(img, grid_r, grid_c);
    if (img.depth() == CV_16U)
        fillGridMatrices<ushort>(img, grid_r, grid_c);
    else if (img.depth() == CV_32F)
        fillGridMatrices<float>(img, grid_r, grid_c);
    else if (img.depth() == CV_64F)
        fillGridMatrices<double>(img, grid_r, grid_c);
}
