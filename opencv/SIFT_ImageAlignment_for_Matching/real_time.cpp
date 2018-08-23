/**
 * @author: mysticalwing
 * @time: 2018-07-3 13:31
 * @brief: use both feature detect and pixel-based image alignment for matching
 */

#include "src/ImageAlignment.h"
#include "src/ImageAlignmentProject.h"
#include "src/ImageAlignmentContainer.h"
#include "src/ImageAlignmentPyramid.h"

#include <opencv2/opencv.hpp>
#include <iostream>

using std::cin;
using std::cout;
using std::endl;

////////////////////////////////////////////////////////////////////////////////////////////////
const cv::Scalar CORNER_COLORS[4] =
    {cv::Scalar(255, 0, 0),
     cv::Scalar(0, 255, 0),
     cv::Scalar(0, 0, 255),
     cv::Scalar(255, 255, 0)};

const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;

const int NUM_LEV = 3;
const int NUM_ITER_PER_SCALE = 5;

const int TEMPLATE_SIZE = 360;

////////////////////////////////////////////////////////////////////////////////////////////////
void getTemplateCorners(const cv::Mat &imageObject, std::vector<cv::Point2f> &corners)
{
    // const cv::Scalar RED_COLOR = cv::Scalar(0, 0, 255);
    // for (int i = 0; i < imageObject.rows; i++)
    // {
    //     const cv::Vec3b *pRow = imageObject.ptr<cv::Vec3b>(i);
    //     for (int j = 0; j < imageObject.cols; j++)
    //     {
    //         const cv::Vec3b &curr = pRow[j];
    //         if ((curr[0] == RED_COLOR[0]) && (curr[1] == RED_COLOR[1]) && (curr[2] == RED_COLOR[2]))
    //         {
    //             corners.push_back(cv::Point2f(j, i));
    //         }
    //     }
    // }
    corners.push_back(cv::Point2f(34, 22));
    corners.push_back(cv::Point2f(247, 17));
    corners.push_back(cv::Point2f(242, 340));
    corners.push_back(cv::Point2f(39, 341));
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool getImage(const std::string &imagePath, cv::Mat &image, const int MAX_SIZE)
{
    image = cv::imread(imagePath, cv::IMREAD_COLOR);

    if (image.empty())
    {
        return false;
    }

    int w = image.cols;
    int h = image.rows;
    int maxSize = std::max(w, h);

    if (MAX_SIZE <= 0)
    {
        return true;
    }
    // resize image if it's size is too large
    if (maxSize > MAX_SIZE)
    {
        double ratio = (double)w / (double)h;
        if (ratio > 1.0)
        {
            w = MAX_SIZE;
            h = w / ratio;
        }
        else
        {
            h = MAX_SIZE;
            w = h * ratio;
        }
        cv::Size size(w, h);
        cv::resize(image, image, size, 0.0, 0.0, cv::INTER_LINEAR);
    }

    return true;
}

void drawBox(cv::Mat &image, const std::vector<cv::Point2f> &corners)
{
    static const int CORNER_NUM = 4;
    if (corners.size() < CORNER_NUM)
    {
        return;
    }
    for (size_t i = 1; i <= CORNER_NUM; i++)
    {
        // cv::circle(image, corners[i], 4, CORNER_COLORS[i], -1);
        cv::line(image, corners[i - 1], corners[i % CORNER_NUM], CORNER_COLORS[2], 2);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char const *argv[])
{
    cout << "Parameters: " << endl
         << "   argv[1]: template image path" << endl;

    if (argc < 2)
    {
        cout << "Too few parameters! At least 2!" << endl;
        return -1;
    }

    // const int large_scene_size = 1080;

    cv::Mat imageObject;
    // get template image
    if (!getImage(argv[1], imageObject, TEMPLATE_SIZE))
    {
        cout << "Template Image read failed!" << endl;
        return -1;
    }

    // open camera
    cv::VideoCapture cap;
    cap.open(0);
    if (!cap.isOpened())
    {
        cout << "Camera open error!" << endl;
        return -1;
    }
    // set camera's attribute
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);

    // gte handle instance
    ImageAlignment handler(imageObject, NUM_LEV, NUM_ITER_PER_SCALE);

    cv::String windowName = "Image Alignment";
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);
    cv::resizeWindow(windowName, cv::Size(FRAME_WIDTH, FRAME_HEIGHT));

    cv::Mat frame, matrixOfScene2Object, im;
    std::vector<cv::Point2f> objCorners;
    getTemplateCorners(imageObject, objCorners);

    cv::Mat temp;
    imageObject.copyTo(temp);
    // for (size_t i = 0; i < objCorners.size(); i++)
    // {
    //     cv::circle(temp, objCorners[i], 2, CORNER_COLORS[2], -1);
    // }
    drawBox(temp, objCorners);
    cv::imshow("Template", temp);
    std::cout << "Template Size: " << temp.size() << std::endl;
    cv::waitKey(0);

    int64 start, end;
    while (true)
    {
        cap >> frame;
        if (frame.empty())
        {
            cout << "Read data error!" << endl;
            break;
        }

        start = cv::getTickCount();
        // handler.calculateMatrixOfImage2Object(frame, matrixOfScene2Object);
        matrixOfScene2Object = handler.calculateMatrixOfImage2Object(frame);
        end = cv::getTickCount();
        std::cout << "Calculate Matrix Costs: " << (end - start) / cv::getTickFrequency() * 1000 << " ms" << std::endl;

        im = matrixOfScene2Object.inv();
        std::vector<cv::Point2f> sceneCorners;
        cv::perspectiveTransform(objCorners, sceneCorners, im);
        // for (size_t i = 0; i < sceneCorners.size(); i++)
        // {
        //     cv::circle(frame, sceneCorners[i], 2, CORNER_COLORS[2], -1);
        // }
        drawBox(frame, sceneCorners);

        // im = handler.H().inv();
        // std::vector<cv::Point2f> sceneCornersSift;
        // cv::perspectiveTransform(objCorners, sceneCornersSift, im);
        // for (size_t i = 0; i < sceneCornersSift.size(); i++)
        // {
        //     cv::circle(frame, sceneCornersSift[i], 2, CORNER_COLORS[0], -1);
        // }

        cv::imshow(windowName, frame);

        char key = cv::waitKey(1) & 0xFF;
        if (key == 27)
        {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}