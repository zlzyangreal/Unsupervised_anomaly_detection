#include "ParseFile.h"
#include <cstdio> // 用于文件操作

void moveFile(const std::string& src, const std::string& dest) {
    // 使用 OpenCV 读取图像
    cv::Mat image = cv::imread(src);
    if (image.empty()) {
        std::cerr << "【Error】Failed to read the image: " << src << std::endl;
        return;
    }

    // 确保目标目录存在
    std::size_t lastSlashPos = dest.find_last_of("/\\");
    if (lastSlashPos != std::string::npos) {
        std::string destDir = dest.substr(0, lastSlashPos);
        std::string command = "mkdir -p " + destDir;
        if (system(command.c_str()) != 0) {
            std::cerr << "【Error】Failed to create directory: " << destDir << std::endl;
            return;
        }
    }

    // 使用 OpenCV 写入图像到目标路径
    if (!cv::imwrite(dest, image)) {
        std::cerr << "【Error】Failed to write the image: " << dest << std::endl;
        return;
    }

    // 删除源文件
    if (std::remove(src.c_str()) != 0) {
        std::cerr << "【Error】Failed to delete the source file: " << src << std::endl;
    }
}
