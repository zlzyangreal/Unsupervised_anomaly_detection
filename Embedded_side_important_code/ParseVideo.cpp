#include "ParseVideo.h"

#include <dirent.h>
#include <vector>

//Camera initialization
int VideCapture_Init(cv::VideoCapture& cap) {
    if (!cap.isOpened()) {
        std::cerr << "【Error】Could not open camera /dev/video0" << std::endl;
        cap.open("/dev/video1");
        if (!cap.isOpened()) {
            std::cerr << "【Error】Could not open camera /dev/video1" << std::endl;
            return -1;
        }
    }
    return 0;
}

int VideoCapture_Save(cv::VideoCapture& cap) {
    cv::Mat frame;
    cv::Mat resizedFrame;
    int width = 1562;
    int height = 1024;
    
    for (int i = 0; i < 20; ++i) {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "【Error】Could not capture frame" << std::endl;
            return -1;
        }
        cv::resize(frame, resizedFrame, cv::Size(width, height));
        std::string filePath = "image/image_" + std::to_string(i + 1) + ".png";
        cv::imwrite(filePath, resizedFrame);
#if DEBUG6
        std::cout << "【Hint】Image saved to " << filePath << std::endl;
#endif
        cv::waitKey(100);
    }
    return 0;
}



int Save_and_show(std::string folderA, std::string folderB, std::string folderC) {
    std::string imageAPath = folderA + "000.png";
    std::string imageBPath = folderB + "000.png";
    std::string imageCPath = folderC + "Result.png";
    cv::Mat imageA = cv::imread(imageAPath);
    cv::Mat imageB = cv::imread(imageBPath);

    if (imageA.empty() || imageB.empty()) {
        std::cerr << "【Error】Failed to load images." << std::endl;
        return 1;
    }

    int newBHeight = static_cast<int>(imageB.rows * (static_cast<double>(imageA.cols) / imageB.cols));
    cv::Mat resizedB;
    cv::resize(imageB, resizedB, cv::Size(imageA.cols, newBHeight));

    int width = imageA.cols; 
    int height = imageA.rows + resizedB.rows; 

    cv::Mat result(height, width, imageA.type());

    cv::Rect roiA(0, 0, imageA.cols, imageA.rows);
    imageA.copyTo(result(roiA));

    cv::Rect roiB(0, imageA.rows, resizedB.cols, resizedB.rows);
    resizedB.copyTo(result(roiB));

    cv::imwrite(imageCPath, result);

#if DEBUG6
    std::cout << "【Hint】Images combined and saved to: " << imageCPath << std::endl;
#endif

    // cv::imshow("Combined Image", result);
    // cv::waitKey(0);

    return 0;
}