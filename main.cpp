#include <iostream>
#include <fstream>
#include "Image.h"
#include <ctime>
#include <chrono>


int main() {
//    clock_t startTime, endTime;
    unsigned long size;
    std::ofstream fout("logs");

    fout << "Without optimization\n";
    for (int i = 1; i <= 15; i++) {
        fout << "===| Image " << i << " |===\n";
        Image image;

        std::string srcPath = "images/";
        std::string destPath = "results/";
        if (i < 10) {
            srcPath += "0";
            destPath += "0";
        }
        srcPath += std::to_string(i) + ".jpg";
        destPath += std::to_string(i) + ".jpg";

        fout << "Reading image...\n";
        image.readJPEG(srcPath.c_str());

        fout << "Read image successfully. Binarizing image...\n";
        auto startTime = std::chrono::steady_clock::now();
        image.runOtsu();
        auto endTime = std::chrono::steady_clock::now();
        size = image.getSize();
        auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        fout << "Binarized image successfully. Writing image...\n";
        image.writeJPEG(100, destPath.c_str());

        fout << "Wrote image successfully\nTotal time: " <<
             (double) totalTime.count() * 1000.0 / (double) size <<  " seconds per MP" << std::endl;
    }

    fout << "\nFirst optimization\n";
    for (int i = 1; i <= 15; i++) {
        fout << "===| Image " << i << " |===\n";
        Image image(200);

        std::string srcPath = "images/";
        std::string destPath = "results2/";
        if (i < 10) {
            srcPath += "0";
            destPath += "0";
        }
        srcPath += std::to_string(i) + ".jpg";
        destPath += std::to_string(i) + ".jpg";

        fout << "Reading image...\n";
        image.readJPEG(srcPath.c_str());

        fout << "Read image successfully. Binarizing image...\n";
        auto startTime = std::chrono::steady_clock::now();
        image.runOtsu();
        auto endTime = std::chrono::steady_clock::now();
        size = image.getSize();
        auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        fout << "Binarized image successfully. Writing image...\n";
        image.writeJPEG(100, destPath.c_str());

        fout << "Wrote image successfully\nTotal time: " <<
             (double) totalTime.count() * 1000.0 / (double) size <<  " seconds per MP" << std::endl;
    }

    fout << "\nSecond optimization\n";
    for (int i = 1; i <= 15; i++) {
        fout << "===| Image " << i << " |===\n";
        Image image(200, true);

        std::string srcPath = "images/";
        std::string destPath = "results3/";
        if (i < 10) {
            srcPath += "0";
            destPath += "0";
        }
        srcPath += std::to_string(i) + ".jpg";
        destPath += std::to_string(i) + ".jpg";

        fout << "Reading image...\n";
        image.readJPEG(srcPath.c_str());

        fout << "Read image successfully. Binarizing image...\n";
        auto startTime = std::chrono::steady_clock::now();
        image.runOtsu();
        auto endTime = std::chrono::steady_clock::now();
        size = image.getSize();
        auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        fout << "Binarized image successfully. Writing image...\n";
        image.writeJPEG(100, destPath.c_str());

        fout << "Wrote image successfully\nTotal time: " <<
             (double) totalTime.count() * 1000.0 / (double) size <<  " seconds per MP" << std::endl;
    }
    fout.close();

    return 0;
}