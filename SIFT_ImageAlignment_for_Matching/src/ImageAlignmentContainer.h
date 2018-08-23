#ifndef IMAGE_ALIGNMENT_CONTAINER_H
#define IMAGE_ALIGNMENT_CONTAINER_H

#include <opencv2/opencv.hpp>

class ImageAlignmentContainer
{
  public:
    /*type define*/
    typedef cv::Matx<double, 8, 8> MatrixA;
    typedef class ImageAlignmentContainer this_type;

    ImageAlignmentContainer(const cv::Mat &image = cv::Mat());

    virtual ~ImageAlignmentContainer();

    this_type &operator=(const this_type &right)
    {
        m_A = right.m_A;
        m_invA = right.m_invA;
        right.m_xIx.copyTo(m_xIx);
        right.m_xIy.copyTo(m_xIy);
        right.m_yIx.copyTo(m_yIx);
        right.m_yIy.copyTo(m_yIy);
        right.m_xG.copyTo(m_xG);
        right.m_yG.copyTo(m_yG);
        right.m_grad_x.copyTo(m_grad_x);
        right.m_grad_y.copyTo(m_grad_y);
        right.m_grid_c.copyTo(m_grid_c);
        right.m_grid_r.copyTo(m_grid_r);
        right.m_image.copyTo(m_image);

        return *this;
    }

  protected:
    void _calculateMembers(const cv::Mat &image);

    void _gradient(const cv::Mat &image, cv::Mat &gradx, cv::Mat &grady);

    void _grid(const cv::Mat &img, cv::Mat &grid_r, cv::Mat &grid_c) const;

    cv::Mat sqr(const cv::Mat &mat1) const
    {
        cv::Mat res;
        res.create(mat1.size(), mat1.type());
        res = mat1.mul(mat1);
        return res;
    }

  public:
    MatrixA m_A;
    MatrixA m_invA;
    cv::Mat m_xIx;
    cv::Mat m_xIy;
    cv::Mat m_yIx;
    cv::Mat m_yIy;
    cv::Mat m_xG;
    cv::Mat m_yG;
    cv::Mat m_grad_x;
    cv::Mat m_grad_y;
    cv::Mat m_grid_c;
    cv::Mat m_grid_r;
    cv::Mat m_image;
};

#endif // IMAGE_ALIGNMENT_CONTAINER_H
