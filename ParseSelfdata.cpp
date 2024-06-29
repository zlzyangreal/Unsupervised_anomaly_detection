#include "SYS.hpp"

#include "ParseInference.h"

using namespace cv;
using namespace std;

Mat Fram(Mat image) {
    // 将彩色图像转换为灰度图像
    Mat gray_image;
    cvtColor(image, gray_image, COLOR_BGR2GRAY);

    // 阈值处理
    Mat thresholded_image;
    threshold(gray_image, thresholded_image, 80, 255, THRESH_BINARY_INV);

    // 膨胀操作
    Mat kernel = getStructuringElement(MORPH_RECT, Size(40, 40));
    Mat dilated_image;
    dilate(thresholded_image, dilated_image, kernel, Point(-1, -1), 1);

    // Canny 边缘检测
    Mat edges;
    Canny(dilated_image, edges, 100, 200);

    // 找到轮廓
    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    findContours(edges, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // 找到最大的旋转外接矩形
    double max_area = 0;
    RotatedRect best_rect;
    std::vector<Point2f> best_box;
    for (const auto& contour : contours) {
        RotatedRect rect = minAreaRect(contour);
        Point2f box[4];
        rect.points(box);
        std::vector<Point2f> box_vec(box, box + 4);
        double area = rect.size.width * rect.size.height;
        if (area > max_area) {
            max_area = area;
            best_rect = rect;
            best_box = box_vec;
        }
    }

    // 画出最大的旋转外接矩形
    if (!best_box.empty()) {
        // 创建一个与原始图像大小相同的空白图像
        Mat image_with_rect = Mat::zeros(image.size(), CV_8UC3);

        // 转换为正确的轮廓类型
        std::vector<std::vector<Point>> contour_vec(1, std::vector<Point>(best_box.begin(), best_box.end()));

        // 绘制轮廓
        drawContours(image_with_rect, contour_vec, 0, Scalar(0, 0, 255), 2);
        // 提取旋转外接矩形
        Point2f center = best_rect.center;
        Size2f size = best_rect.size;
        float angle = best_rect.angle;

        // 获取旋转矩阵
        Mat rotation_matrix = getRotationMatrix2D(center, angle, 1.0);
        Mat rotated_image;
        warpAffine(image, rotated_image, rotation_matrix, image.size(), INTER_CUBIC);

        // 提取旋转后的图像
        Mat extracted_image;
        getRectSubPix(rotated_image, size, center, extracted_image);

        // 检测提取图像的宽高比
        int width = extracted_image.cols;
        int height = extracted_image.rows;
        if (width < height) {
            // 如果宽度小于高度，则需要旋转图像90度
            transpose(extracted_image, extracted_image);
            flip(extracted_image, extracted_image, 1);
        }

        // 保存图像到文件
        imwrite("Data/temp/.png", extracted_image);
        return extracted_image;
    } else {
        std::cout << "【Error】No contours found." << std::endl;
    }
}

int Check(const Mat& image) {
    string outputPath = "Data/temp/b.png";
    // 将图像转换为灰度图像
    Mat grayImage;
    cvtColor(image, grayImage, COLOR_BGR2GRAY);

    // 二值化图像
    Mat binaryImage;
    threshold(grayImage, binaryImage, 128, 255, THRESH_BINARY);

    // 保存二值图像
    imwrite(outputPath, binaryImage);

    // 计算图像中心分开两边的黑色占比
    int centerColumn = binaryImage.cols / 2;
    int totalPixels = binaryImage.rows * binaryImage.cols;
    int leftBlackPixels = 0;
    int rightBlackPixels = 0;

    for (int row = 0; row < binaryImage.rows; ++row)
    {
        for (int col = 0; col < binaryImage.cols; ++col)
        {
            if (col < centerColumn && binaryImage.at<uchar>(row, col) == 0)
                leftBlackPixels++;
            else if (col > centerColumn && binaryImage.at<uchar>(row, col) == 0)
                rightBlackPixels++;
        }
    }

    double leftBlackRatio = static_cast<double>(leftBlackPixels) / totalPixels;
    double rightBlackRatio = static_cast<double>(rightBlackPixels) / totalPixels;

#if DEBUG6
    // 输出结果
    cout << "左侧黑色占比: " << leftBlackRatio << endl;
    cout << "右侧黑色占比: " << rightBlackRatio << endl;
#endif

    if(leftBlackRatio<rightBlackRatio) {
        return 1;
    } else {
        return 0;
    } 
}

cv::Mat Spin(std::string image_path) {
    // 使用cv::Mat读取图片
    cv::Mat image = cv::imread(image_path, cv::IMREAD_COLOR);
    // 检查图片是否成功加载
    if (image.empty()) {
        std::cerr << "【Error】Could not read the image: " << image_path << std::endl;
    }
    // 旋转图片180度
    cv::Mat rotated_image;
    cv::flip(image, rotated_image, -1); // -1 表示旋转180度
    // 保存旋转后的图片
    cv::imwrite("Data/temp/s.png", rotated_image);
    return rotated_image;
}

int Self_Data_Deal() {
    // 读取图像
    Mat image = imread("Data/test/000.png");//0004
    Mat imageFrame = Fram(image);
    if (imageFrame.empty())
    {
        cout << "【Error】Unable to read the image file" << endl;
        return -1;
    }
    int temp = Check(imageFrame);
    cv::Mat endImage;
    if(temp==0) {
        std::string image_path = "Data/temp/.png";
        endImage = Spin(image_path);
    }else {
        endImage = imageFrame;
    }
    cv::imwrite("Data/temp/e.png",endImage);

    Inference();
    
    return 0;
}