//
// Created by michael on 24.04.18.
//

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include "Image.h"

Image::Image() {
    method = "luminosity";
}

Image::~Image() {}

void Image::setMethod(const char* method) {
    this->method = method;
}

int Image::toGray(int red, int green, int blue) {
    if (method == "luminosity") {
        return (int)(0.21 * red + 0.72 * green + 0.07 * blue);
    }
    if (method == "average") {
        return (red + green + blue) / 3;
    }
    if (method == "lightness") {
        return (std::min(std::min(red, green), blue) +
                std::max(std::max(red, green), blue)) / 2;
    }
}

std::vector<int> Image::getIntensityGrayVector() {
    std::vector<int> result;
    for (int i = 0; i < pixels.size(); i += 3) {
        result.push_back(toGray((int)pixels[i], (int)pixels[i+1], (int)pixels[i+2]));
    }
    return result;
}

void Image::convertToGray(std::vector<int> grayValues) {
    if (grayValues.size() * 3 != pixels.size()) {
        throw std::invalid_argument("Wrong array size\n");
    }
    for (int i = 0; i < grayValues.size(); i++) {
        pixels[3 * i] = (unsigned char)grayValues[i];
        pixels[3 * i + 1] = (unsigned char)grayValues[i];
        pixels[3 * i + 2] = (unsigned char)grayValues[i];
    }
}

void Image::convertToBin(std::vector<int> binValues) {
    if (binValues.size() * 3 != pixels.size()) {
        throw std::invalid_argument("Wrong array size\n");
    }
    for (int i = 0; i < binValues.size(); i++) {
        pixels[3 * i] = (unsigned char)(binValues[i] * 255);
        pixels[3 * i + 1] = (unsigned char)(binValues[i] * 255);
        pixels[3 * i + 2] = (unsigned char)(binValues[i] * 255);
    }
}

std::vector<int> Image::calculateHist(const std::vector<int>& intensityVector) {
    std::vector<int> result(intensity, 0);
    for (int i = 0; i < intensityVector.size(); i++) {
        result[intensityVector[i]]++;
    }
    return result;
}

unsigned long Image::calculateIntensitySum(const std::vector<int>& intensityVector) {
    unsigned long result = 0;
    for (int i = 0; i < intensityVector.size(); i++) {
        result += intensityVector[i];
    }
    return result;
}

int Image::otsuThreshold() {
    std::vector<int> gray = getIntensityGrayVector();

    convertToGray(gray);

    std::vector<int> hist = calculateHist(gray);
    unsigned long sum = calculateIntensitySum(gray);
    unsigned long pixelCount = width * height;

    int bestThresh = 0;
    double bestSigma = 0.0;

    int firstClassPixelCount = 0;
    int firstClassIntensitySum = 0;

    for (int i = 0; i < 255; i++) {
        firstClassPixelCount += hist[i];
        firstClassIntensitySum += i * hist[i];

        double firstClassProb = firstClassPixelCount / (double) pixelCount;
        double secondClassProb = 1.0 - firstClassProb;

        double firstClassMean = firstClassIntensitySum / (double) firstClassPixelCount;
        double secondClassMean = (sum - firstClassIntensitySum)
                                 / (double) (pixelCount - firstClassPixelCount);

        double deltaMean = firstClassMean - secondClassMean;

        double sigma = firstClassProb * secondClassProb * deltaMean * deltaMean;

        if (sigma > bestSigma) {
            bestSigma = sigma;
            bestThresh = i;
        }
    }

    return bestThresh;
}

void Image::binarize(int threshold) {
    for (int i = 0; i < width * height * 3; i++) {
        if (pixels[i] < threshold) {
            pixels[i] = 0;
        }
        else {
            pixels[i] = 255;
        }
    }
}

void Image::runOtsu() {
    int threshold = otsuThreshold();
    binarize(threshold);
}

void Image::readJPEG(const char *path) {
    FILE *file = fopen(path, "rb");
    if (file == nullptr) {
        std::cout << "Cannot read image";
    }

    struct jpeg_decompress_struct info;
    struct jpeg_error_mgr err;

    info.err = jpeg_std_error(&err);
    jpeg_create_decompress(&info);

    jpeg_stdio_src(&info, file);
    jpeg_read_header(&info, true);

    jpeg_start_decompress(&info);

    width = info.output_width;
    height = info.output_height;
    int numChannels = info.num_components;
    unsigned long dataSize = width * height * numChannels;

    auto data = (unsigned char*)malloc(dataSize);
    unsigned char* rowptr;
    while (info.output_scanline < height) {
        rowptr = data + info.output_scanline * width * numChannels;
        jpeg_read_scanlines(&info, &rowptr, 1);
    }

    jpeg_finish_decompress(&info);

    fclose(file);

    pixels.resize(dataSize);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            for (int c = 0; c < numChannels; c++) {
                pixels[y * width * numChannels + x * numChannels + c] =
                        data[y * width * numChannels + x * numChannels + c];
            }
        }
    }

    free(data);
}

void Image::writeJPEG(int quality, const char* path) {
    /* Compress to JPEG */
    auto *scr=(char*)malloc(width*height*3* sizeof(char));
    for (int i = 0; i < width*height*3; i++) {
        scr[i] = pixels[i];
    }


    FILE * file = fopen(path, "wb");
    if (file == nullptr) {
        throw std::invalid_argument("Wrong output path");
    }
    struct jpeg_compress_struct info;
    struct jpeg_error_mgr err;
    info.err = jpeg_std_error(&err);
    jpeg_create_compress(&info);

    jpeg_stdio_dest(&info, file);

    info.image_width = width;
    info.image_height = height;
    info.input_components = 3;
    info.in_color_space = JCS_RGB;

    jpeg_set_defaults(&info);
    jpeg_set_quality(&info, quality, true);
    jpeg_start_compress(&info, TRUE);

    JSAMPROW row_pointer[1];
    int row_stride;

    row_stride = info.image_width*3;

    while (info.next_scanline < info.image_height) {
        row_pointer[0]=(JSAMPLE *)(scr+info.next_scanline*row_stride);
        jpeg_write_scanlines(&info, row_pointer, 1);
    }
    jpeg_finish_compress(&info);
    jpeg_destroy_compress(&info);

    free(scr);
}