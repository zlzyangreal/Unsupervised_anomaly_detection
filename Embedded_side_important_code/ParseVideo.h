#ifndef _PARSE_VIDEO_H_
#define _PARSE_VIDEO_H_

#include "SYS.hpp"

int VideCapture_Init(cv::VideoCapture& cap);
int VideoCapture_Save(cv::VideoCapture& cap);

int Save_and_show(std::string folderA,std::string folderB, std::string folderC);

#define Single_image_mode 0

#endif