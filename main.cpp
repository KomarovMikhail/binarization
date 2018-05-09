#include <iostream>
#include "Image.h"


int main() {
    for (int i = 1; i <= 15; i++) {
        std::cout << "===| Image " << i << " |===\n";
        Image image;

        std::string srcPath = "images/";
        std::string destPath = "results/";
        if (i < 10) {
            srcPath += "0";
            destPath += "0";
        }
        srcPath += std::to_string(i) + ".jpg";
        destPath += std::to_string(i) + ".jpg";

        std::cout << "Reading image...\n";
        image.readJPEG(srcPath.c_str());

        std::cout << "Read image successfully. Binarizing image...\n";
        image.runOtsu();

        std::cout << "Binarized image successfully. Writing image...\n";
        image.writeJPEG(100, destPath.c_str());

        std::cout << "Wrote image successfully\n";
    }

    for (int i = 1; i <= 15; i++) {
        std::cout << "===| Image " << i << " |===\n";
        Image image(200);

        std::string srcPath = "images/";
        std::string destPath = "results2/";
        if (i < 10) {
            srcPath += "0";
            destPath += "0";
        }
        srcPath += std::to_string(i) + ".jpg";
        destPath += std::to_string(i) + ".jpg";

        std::cout << "Reading image...\n";
        image.readJPEG(srcPath.c_str());

        std::cout << "Read image successfully. Binarizing image...\n";
        image.runOtsu();

        std::cout << "Binarized image successfully. Writing image...\n";
        image.writeJPEG(100, destPath.c_str());

        std::cout << "Wrote image successfully\n";
    }

    return 0;
}