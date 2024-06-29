#include "ParseFile.h"
#include <cstdio> 

void moveFile(const std::string& src, const std::string& dest) {
    cv::Mat image = cv::imread(src);
    if (image.empty()) {
        std::cerr << "【Error】Failed to read the image: " << src << std::endl;
        return;
    }

    std::size_t lastSlashPos = dest.find_last_of("/\\");
    if (lastSlashPos != std::string::npos) {
        std::string destDir = dest.substr(0, lastSlashPos);
        std::string command = "mkdir -p " + destDir;
        if (system(command.c_str()) != 0) {
            std::cerr << "【Error】Failed to create directory: " << destDir << std::endl;
            return;
        }
    }

    if (!cv::imwrite(dest, image)) {
        std::cerr << "【Error】Failed to write the image: " << dest << std::endl;
        return;
    }

    if (std::remove(src.c_str()) != 0) {
        std::cerr << "【Error】Failed to delete the source file: " << src << std::endl;
    }
}
