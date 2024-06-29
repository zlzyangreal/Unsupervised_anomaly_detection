#include "SYS.hpp"

#include <unistd.h> 
#include <iostream>

#include "ParseVideo.h"
#include "ParseInterrnce.h" 
#include "ParseFile.h"
#include "ParseSelfdata.h"

int main_model = 0;

// Define the project path
const std::string basePath = "";

const std::string efficientADPath = basePath + "Efficient/";
const std::string admmRegADPath = basePath + "ADMM-RegAD/";
const std::string machineVisionDataPath = basePath + "Data/";
const std::string resultPath = basePath + "Result/";

const std::string Efficient_AD_folderA = efficientADPath + "Start/";
const std::string Efficient_AD_folderB = efficientADPath + "Result/";

const char* ADMM_RegAD_newWorkingDir = admmRegADPath.c_str();
const std::string ADMM_RegAD_folderA = admmRegADPath + "Start/";
const std::string ADMM_RegAD_folderB = admmRegADPath + "Result/";

const std::string Machine_vision_folderA = machineVisionDataPath + "test/";
const std::string Machine_vision_folderB = machineVisionDataPath + "temp/";

const std::string Result_PATH = resultPath;


int TheModelFunction(void) {
    std::cout << "【Hint】Please select your operating mode" << std::endl;
    std::cout << "[ Note ]:0-Efficient-AD   1-Machine_vision   2-ADMM_RD(Self-developed algorithm)" <<std::endl;
    std::cout << "Please enter: ";
    std::cin >> main_model ;
    std::cout << "【Hint】You chose model " << main_model <<std::endl;
}

int main(void) {
    int ret;
    TheModelFunction();
#if DEBUG1
    if(main_model==1||main_model==2) {
        cv::VideoCapture cap("/dev/video0");
        ret = VideCapture_Init(cap);
        if(ret==0) {
            std::cout << "【Hint】Camera opened successfully" <<std::endl;
        }else {
            std::cout << "【Error】Failed to open camera" <<std::endl;
            return -1;
        }
        ret = VideoCapture_Save(cap);
        #if DEBUG6
        if(ret==0) {
            std::cout << "【Hint】Image saved successfully" << std::endl;
        } else {
            std::cout << "【Error】Failed to save image" <<std::endl;
        }
        #endif
        cap.release();
    } else {
        std::cout << "【Hint】The mode is used to detect public datasets" << std::endl;
    }
#endif
#if DEBUG2
    if(main_model==1) {
        const std::string srcPath = basePath + "image/image_15.png";
        const std::string destPath = basePath + "Data/test/000.png";
        moveFile(srcPath, destPath);
        #if DEBUG6
        std::cout << "File moved successfully" << std::endl;
        #endif
    } else if(main_model==2){
        const std::string srcPath = "image/image_15.png";
        const std::string destPath = "ADMM-RegAD/Start/000.png";
        moveFile(srcPath, destPath);
        #if DEBUG6
        std::cout << "File moved successfully" << std::endl;
        #endif
    } else {
        std::cout << "【Hint】You can choose the test image" << std::endl;
    }

#endif

#if DEBUG5
    if(main_model==1) {
        Self_Data_Deal();
    } else {
        std::cout << "【Hint】The mode is deep learning mode" << std::endl;
    }
#endif

#if DEBUG3
    if(main_model==0) {
        ret = Efficient_AD_Inference();
        if(ret==0) {
            std::cout << "【Hint】Inference successfully" << std::endl;
        } else {
            std::cout << "【Error】Inference failure" << std::endl;
            return 1;
        }
    } else if(main_model==2){
        ret = ADMM_RegAD_Inference();
        if(ret==0) {
            std::cout << "【Hint】Inference successfully" << std::endl;
        } else {
            std::cout << "【Error】Inference failure" << std::endl;
            return 1;
        }
    } else {
        std::cout << "【Hint】The mode is traditional machine vision mode" << std::endl;
    }
#endif
#if DEBUG4
    if(main_model==0) {
        std::string folderA = Efficient_AD_folderA;
        std::string folderB = Efficient_AD_folderB;
        std::string folderC = Result_PATH;  
        ret = Save_and_show(folderA,folderB,folderC);
    } else if(main_model==2){
        std::string folderA = ADMM_RegAD_folderA;
        std::string folderB = ADMM_RegAD_folderB;
        std::string folderC = Result_PATH;  
        ret = Save_and_show(folderA,folderB,folderC);
    } else {
        std::string folderA = Machine_vision_folderA;
        std::string folderB = Machine_vision_folderB;
        std::string folderC = Result_PATH;
        ret = Save_and_show(folderA,folderB,folderC);
    }
    if(ret!=0) {
        std::cout <<"【Error】Failed to save the display result" << std::endl;
    } else {
        std::cout <<"【Hint】Save the display result successfully" << std::endl;
    }
#endif
    return 0;
}
