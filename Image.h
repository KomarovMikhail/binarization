//
// Created by michael on 24.04.18.
//

#ifndef BINARIZATION_IMAGE_H
#define BINARIZATION_IMAGE_H

#include <vector>
#include <string>

extern "C"
{
#include <jpeglib.h>
}

class Image {
    unsigned long width, height;
    std::vector<unsigned char> pixels;
    const int intensity = 256;

    /* Method, used to convert color image to gray scale
     * "lightness", "average", "luminosity"
     * default: "luminosity" */
    std::string method;

    int toGray(int red, int green, int blue);
    std::vector<int> getIntensityGrayVector();
    std::vector<int> calculateHist(const std::vector<int>& intensityVector);
    unsigned long calculateIntensitySum(const std::vector<int>& intensityVector);
    void convertToGray(std::vector<int> grayValues);
    void convertToBin(std::vector<int> binValues);
    int otsuThreshold();
    void binarize(int threshold);

public:
    explicit Image();
    ~Image();

    void readJPEG(const char *path);
    void writeJPEG(int quality, const char* path);
    void setMethod(const char* method);
    void runOtsu();
};

#endif //BINARIZATION_IMAGE_H
