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
    unsigned long width, height, blockLength;
    std::vector<unsigned char> pixels;
    const int intensity = 256;

    /* Method, used to convert color image to gray scale
     * "lightness", "average", "luminosity"
     * default: "luminosity" */
    std::string method;
    std::vector<int> intensityVector;
    std::vector<int> binValues;

    int toGray(int red, int green, int blue);
    void getIntensityGrayVector();
    std::vector<int> calculateHist(unsigned long corner);
    unsigned long calculateIntensitySum(unsigned long corner);
    void convertToGray();
    void convertToBin();
    int otsuThreshold(unsigned long corner);
    void getBinValues(int threshold, unsigned long corner);
    unsigned long getPixelCount(unsigned long corner);

public:
    explicit Image();
    explicit Image(unsigned long d);
    ~Image();

    void readJPEG(const char *path);
    void writeJPEG(int quality, const char* path);
    void setMethod(const char* method);
    void runOtsu();
};

#endif //BINARIZATION_IMAGE_H
