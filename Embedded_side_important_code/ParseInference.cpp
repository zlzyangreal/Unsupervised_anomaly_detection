#include "ParseInference.h"
#include <vector>
#include <string>
#include <dirent.h>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

void saveImage(const string& path, const Mat& image) {
    imwrite(path, image);
}

double calculateSSIM(const Mat& img1, const Mat& img2) {
    Mat img1_gray, img2_gray;
    cvtColor(img1, img1_gray, COLOR_BGR2GRAY);
    cvtColor(img2, img2_gray, COLOR_BGR2GRAY);
    
    double C1 = 6.5025, C2 = 58.5225;
    Mat I1, I2;
    img1_gray.convertTo(I1, CV_32F);
    img2_gray.convertTo(I2, CV_32F);

    Mat I2_2 = I2.mul(I2);  
    Mat I1_2 = I1.mul(I1);  
    Mat I1_I2 = I1.mul(I2); 

    Mat mu1, mu2;   
    GaussianBlur(I1, mu1, Size(11, 11), 1.5);
    GaussianBlur(I2, mu2, Size(11, 11), 1.5);

    Mat mu1_2 = mu1.mul(mu1);
    Mat mu2_2 = mu2.mul(mu2);
    Mat mu1_mu2 = mu1.mul(mu2);

    Mat sigma1_2, sigma2_2, sigma12;

    GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
    sigma1_2 -= mu1_2;

    GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
    sigma2_2 -= mu2_2;

    GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
    sigma12 -= mu1_mu2;

    Mat t1, t2, t3;

    t1 = 2 * mu1_mu2 + C1;
    t2 = 2 * sigma12 + C2;
    t3 = t1.mul(t2);

    t1 = mu1_2 + mu2_2 + C1;
    t2 = sigma1_2 + sigma2_2 + C2;
    t1 = t1.mul(t2);

    Mat ssim_map;
    divide(t3, t1, ssim_map);
    Scalar mssim = mean(ssim_map);

    return mssim[0];
}

vector<string> getFilesInDirectory(const string& dirPath) {
    vector<string> files;
    DIR* dir = opendir(dirPath.c_str());
    if (dir == nullptr) {
        cerr << "[fatal error]Failed to open directory: " << dirPath << endl;
        return files;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        string fileName = entry->d_name;
        if (fileName != "." && fileName != ".." && fileName.find(".png") != string::npos) {
            files.push_back(dirPath + "/" + fileName);
        }
    }
    closedir(dir);
    return files;
}

static int maxInliers = 0;
static string maxInliersImageName;


void processImage(const string& imagePath, const Mat& testImage, const vector<KeyPoint>& testKeypoints, const Mat& testDescriptors, const string& outputDir, Mat& img2_aligned) {
    Mat img2 = imread(imagePath);
    if (img2.empty()) {
        cerr << "[fatal error]Failed to load image: " << imagePath << endl;
        return;
    }

    Mat img2_gray;
    cvtColor(img2, img2_gray, COLOR_BGR2GRAY);
    Mat descriptors2, img2_keypoints;
    Ptr<ORB> orb = ORB::create();
    vector<KeyPoint> keypoints2;
    orb->detectAndCompute(img2_gray, Mat(), keypoints2, descriptors2);

    BFMatcher matcher(NORM_HAMMING);
    vector<DMatch> matches;
    matcher.match(testDescriptors, descriptors2, matches);

    Mat matchesImage;
    drawMatches(testImage, testKeypoints, img2, keypoints2, matches, matchesImage);
#if DEBUG6
    saveImage(outputDir + "matches_" + imagePath.substr(imagePath.find_last_of("/\\") + 1), matchesImage);
#endif
    vector<Point2f> points1, points2;
    for (const auto& match : matches) {
        points1.push_back(testKeypoints[match.queryIdx].pt);
        points2.push_back(keypoints2[match.trainIdx].pt);
    }

    Mat mask;
    Mat H = findHomography(points1, points2, RANSAC, 3, mask);
    if (H.empty()) {
        cerr << "[fatal error]Failed to find homography for " << imagePath << endl;
        return;
    }

    vector<DMatch> inliers;
    for (size_t i = 0; i < matches.size(); i++) {
        if (mask.at<uchar>(i)) {
            inliers.push_back(matches[i]);
        }
    }

    // Print the number of inliers
#if DEBUG6
    cout << "Number of inliers: " << inliers.size() << endl;
#endif
    if (inliers.size() > maxInliers) {
        maxInliers = inliers.size();
        maxInliersImageName = imagePath.substr(imagePath.find_last_of("/\\") + 1);
    }

    Mat inliersImage;
    drawMatches(testImage, testKeypoints, img2, keypoints2, inliers, inliersImage);
#if DEBUG6
    saveImage(outputDir + "inliers_" + imagePath.substr(imagePath.find_last_of("/\\") + 1), inliersImage);
#endif
    warpPerspective(img2, img2_aligned, H, testImage.size());
    // saveImage(outputDir + "aligned_" + imagePath.substr(imagePath.find_last_of("/\\") + 1), img2_aligned);
}


std::string findCorrespondingMaskImage(const std::string& maxInliersImageName, const std::string& maskFolderPath)
{
    std::string maxInliersDigits = maxInliersImageName.substr(maxInliersImageName.length() - 7, 3); // 提取后三个数字
    std::string maskImageName = maxInliersDigits + "_mask.png";

    DIR* dir;
    struct dirent* entry;

    if ((dir = opendir(maskFolderPath.c_str())) != NULL)
    {
        while ((entry = readdir(dir)) != NULL)
        {
            std::string filename = entry->d_name;
            if (filename == maskImageName)
            {
                closedir(dir);
                return maskFolderPath + "/" + filename;
            }
        }
        closedir(dir);
    }

    return "";  
}

void resizeAndConcatenateImages(const std::string& imagePath1, const std::string& imagePath2, const std::string& imagePath3, const std::string& outputFolderPath) {
    // Load images
    cv::Mat image1 = cv::imread(imagePath1);
    cv::Mat image2 = cv::imread(imagePath2);
    cv::Mat image3 = cv::imread(imagePath3);

    if (image1.empty() || image2.empty() || image3.empty()) {
        std::cout << "[fatal error]Failed to load images." << std::endl;
        return;
    }

    // Define target size for resizing
    cv::Size targetSize(900, 290);

    // Resize images
    cv::resize(image1, image1, targetSize);
    cv::resize(image2, image2, targetSize);
    cv::resize(image3, image3, targetSize);

    // Concatenate images horizontally
    cv::Mat tempConcatenatedImage;
    cv::hconcat(image1, image2, tempConcatenatedImage);
    cv::Mat finalConcatenatedImage;
    cv::hconcat(tempConcatenatedImage, image3, finalConcatenatedImage);

    // Save the concatenated image
    std::string outputFilePath = outputFolderPath + "/000.png";
    cv::imwrite(outputFilePath, finalConcatenatedImage);
#if DEBUG6
    std::cout << "Concatenated image saved to: " << outputFilePath << std::endl;
#endif
}


void extractAndSaveRegion(const std::string& imageAPath, const std::string& maskBPath, const std::string& savePath) {
    // Load images
    cv::Mat imageA = cv::imread(imageAPath);
    cv::Mat maskB = cv::imread(maskBPath, cv::IMREAD_GRAYSCALE); // Ensure mask is loaded in grayscale

    if (imageA.empty()) {
        std::cerr << "[fatal error]Could not load image A from " << imageAPath << std::endl;
        return;
    }
    if (maskB.empty()) {
        std::cerr << "[fatal error]Could not load mask B from " << maskBPath << std::endl;
        return;
    }

    // Resize maskB to the size of imageA
    cv::Mat resizedMaskB;
    cv::resize(maskB, resizedMaskB, imageA.size());

    // Ensure mask is binary (0 or 255)
    cv::Mat binaryMask;
    cv::threshold(resizedMaskB, binaryMask, 128, 255, cv::THRESH_BINARY);

    // Create an output image
    cv::Mat outputImage;
    imageA.copyTo(outputImage, binaryMask);

    // Save the resulting image
    cv::imwrite(savePath, outputImage);
#if DEBUG6
    std::cout << "Result saved to " << savePath << std::endl;
#endif
}

int Inference() {
    string testImagePath = "Data/temp/e.png";
    string referenceDirPath = "Data/refer/";
    string outputDirPath = "Data/temp/";
    std::string maskFolderPath = "Data/mask";  // 替换为实际的mask文件夹路径



    Mat img1 = imread(testImagePath);
    if (img1.empty()) {
        cerr << "[fatal error]Failed to load test image!" << endl;
        return -1;
    }
    Mat img1_gray;
    cvtColor(img1, img1_gray, COLOR_BGR2GRAY);

    Ptr<ORB> orb = ORB::create();
    vector<KeyPoint> keypoints1;
    Mat descriptors1;
    orb->detectAndCompute(img1_gray, Mat(), keypoints1, descriptors1);

    Mat img1_keypoints;
    drawKeypoints(img1, keypoints1, img1_keypoints);
#if DEBUG6
    saveImage(outputDirPath + "test_image_keypoints.png", img1_keypoints);
#endif
    vector<string> referenceImages = getFilesInDirectory(referenceDirPath);
    Mat img2_aligned;
    for (const string& referenceImagePath : referenceImages) {
        processImage(referenceImagePath, img1, keypoints1, descriptors1, outputDirPath, img2_aligned);

        double ssim = calculateSSIM(img1, img2_aligned);
#if DEBUG6
        cout << "SSIM between " << referenceImagePath.substr(referenceImagePath.find_last_of("/\\") + 1)
             << " and test image: " << ssim << endl;
#endif
    }
#if DEBUG6
    cout << "Image with maximum inliers: " << maxInliersImageName << endl;
#endif
    std::string correspondingMaskImage = findCorrespondingMaskImage(maxInliersImageName, maskFolderPath);
#if DEBUG6
    cout << "Corresponding mask image found: " <<correspondingMaskImage <<endl;
#endif
    std::string savePath = "Data/temp/result.png";
    extractAndSaveRegion(testImagePath,correspondingMaskImage,savePath);

    resizeAndConcatenateImages(testImagePath,correspondingMaskImage,savePath,outputDirPath);

    return 0;
}