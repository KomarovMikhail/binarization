#include <cstdio>
#include <iostream>
#include <cstdlib>
#include "Image.h"
#include <cmath>

Image::Image() {
    method = "luminosity";
    blockLength = 0;
    useDeviation = false;
}

Image::Image(unsigned long d) {
    method = "luminosity";
    blockLength = d;
    useDeviation = false;
}

Image::Image(unsigned long d, bool deviation) {
    method = "luminosity";
    blockLength = d;
    useDeviation = deviation;
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

void Image::getIntensityGrayVector() {
    for (int i = 0; i < pixels.size(); i += 3) {
        intensityVector.push_back(toGray((int)pixels[i], (int)pixels[i+1], (int)pixels[i+2]));
    }
}

void Image::convertToGray() {
    if (intensityVector.size() * 3 != pixels.size()) {
        throw std::invalid_argument("Wrong array size\n");
    }
    for (int i = 0; i < intensityVector.size(); i++) {
        pixels[3 * i] = (unsigned char)intensityVector[i];
        pixels[3 * i + 1] = (unsigned char)intensityVector[i];
        pixels[3 * i + 2] = (unsigned char)intensityVector[i];
    }
}

void Image::convertToBin() {
    if (binValues.size() * 3 != pixels.size()) {
        throw std::invalid_argument("Wrong array size\n");
    }
    for (int i = 0; i < binValues.size(); i++) {
        pixels[3 * i] = (unsigned char)(binValues[i] * 255);
        pixels[3 * i + 1] = (unsigned char)(binValues[i] * 255);
        pixels[3 * i + 2] = (unsigned char)(binValues[i] * 255);
    }
}

std::vector<int> Image::calculateHist(unsigned long corner) {
    std::vector<int> result(intensity, 0);
    for (unsigned long i = 0; i < width - corner % width && i < blockLength; i++) {
        for (unsigned long j = 0; j < height - corner / width && j < blockLength; j++) {
            result[intensityVector[corner + i + j * width]]++;
        }
    }
    return result;
}

unsigned long Image::calculateIntensitySum(unsigned long corner) {
    unsigned long result = 0;
    for (unsigned long i = 0; i < width - corner % width && i < blockLength; i++) {
        for (unsigned long j = 0; j < height - corner / width && j < blockLength; j++) {
            result += intensityVector[corner + i + j * width];
        }
    }
    return result;
}

unsigned long Image::getPixelCount(unsigned long corner){
    if (width - corner % width < blockLength && height - corner / width < blockLength) {
        return (width - corner % width) * (height - corner / width);
    }
    if (width - corner % width >= blockLength && height - corner / width < blockLength) {
        return blockLength * (height - corner / width);
    }
    if (width - corner % width < blockLength && height - corner / width >= blockLength) {
        return (width - corner % width) * blockLength;
    }
    if (width - corner % width >= blockLength && height - corner / width >= blockLength) {
        return blockLength * blockLength;
    }
}

unsigned long Image::calculateDeviation(unsigned long corner, long mean) {
    unsigned long result = 0;
    for (unsigned long i = 0; i < width - corner % width && i < blockLength; i++) {
        for (unsigned long j = 0; j < height - corner / width && j < blockLength; j++) {
            result += std::abs(mean - (long)intensityVector[corner + i + j * width]);
        }
    }
    return result;
}

int Image::otsuThreshold(unsigned long corner) {

    std::vector<int> hist = calculateHist(corner);
    unsigned long sum = calculateIntensitySum(corner);
    unsigned long pixelCount = getPixelCount(corner);

    if (useDeviation) {
        long mean = sum / pixelCount;
        unsigned long deviation = calculateDeviation(corner, mean);
        if (deviation < 180000) {
            return 128;
        }
    }

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

void Image::getBinValues(int threshold, unsigned long corner) {
    for (unsigned long i = 0; i < width - corner % width && i < blockLength; i++) {
        for (unsigned long j = 0; j < height - corner / width && j < blockLength; j++) {
            if (intensityVector[corner + i + j * width] < threshold) {
                binValues[corner + i + j * width] = 0;
            }
            else {
                binValues[corner + i + j * width] = 1;
            }
        }
    }
}

void Image::runOtsu() {
    getIntensityGrayVector();
    convertToGray();
    binValues.resize(height * width);

    int threshold;
    if (blockLength == 0) {
        blockLength = std::max(width, height);
        threshold = otsuThreshold(0);
        getBinValues(threshold, 0);
    }
    else {
        for (int i = 0; i < width; i += blockLength) {
            for (int j = 0; j < height; j += blockLength) {
                unsigned long corner = i + j * width;
                threshold = otsuThreshold(corner);
                getBinValues(threshold, corner);
            }
        }
    }
    convertToBin();
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

unsigned long Image::getSize() {
    return height * width;
}