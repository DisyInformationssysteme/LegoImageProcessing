#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include "lib/stb_image.h"
#include "lib/stb_image_write.h"
#include "Data.h"
#include "Color.h"
#include "Draw.h"

using namespace std;

int main() {
    vector<ColorClass> calibrationColors = {ColorClass::DarkBlue, ColorClass::Purple, ColorClass::Cyan,
                                            ColorClass::Green, ColorClass::DarkGrey, ColorClass::White,
                                            ColorClass::Pink, ColorClass::Black, ColorClass::Red, ColorClass::Brown};
    char const* inFileName = "/home/galetzka/dev/LegoImageProcessing/test/02_extract.jpg";
    char const* outFileName = "/home/galetzka/dev/LegoImageProcessing/test/02_out.bmp";
    int width, height, channels;
    cout << "Reading image..." << endl;
    PixelRGB *data = stbi_load(inFileName, &width, &height, &channels, 3);
    int pixelCount = width * height;
    if (data == nullptr) {
        cout << "No file found / error processing image" << endl;
        return -1;
    }
    cout << "Width: " << width << ", Height: " << height << ", Channels: " << channels << endl;


    cout << "Converting to HSV..." << endl;
    vector<PixelHSV> hsvData(pixelCount);
    for (int i = 0; i < pixelCount; i++) {
        PixelRGB r = data[i * 3 + 0];
        PixelRGB g = data[i * 3 + 1];
        PixelRGB b = data[i * 3 + 2];
        hsvData[i] = Color::toHSV(r, g, b);
    }
    Image image = {width, height, hsvData};

    cout << "Moving window calibration..." << endl;
    for (int windowSize = 50; windowSize < 300; windowSize += 50) {
        for (int wX = 0; wX < (width - 1) / windowSize; wX++) {
            for (int wY = 0; wY < (height - 1) / windowSize; wY++) {
                PixelXY upperLeft = {wX * windowSize, wY * windowSize};
                PixelXY lowerRight = {upperLeft.x + windowSize, upperLeft.y + windowSize};
                //Draw::rectangle(image, upperLeft, lowerRight, {windowSize, 1, 1});
            }
        }
    }


    for (int i = 0; i < pixelCount; i++) {
        auto pixelClass = Color::classify(image.data[i]);
        if (pixelClass == ColorClass::Cyan) {
            image.data[i] = {0, 1, 1};
        } else if (pixelClass == ColorClass::DarkBlue) {
            image.data[i] = {30, 1, 1};
        } else if (pixelClass == ColorClass::GreyBoard) {
            image.data[i] = {60, 1, 1};
        } else if (pixelClass == ColorClass::Purple) {
            image.data[i] = {90, 1, 1};
        } else if (pixelClass == ColorClass::Green) {
            image.data[i] = {120, 1, 1};
        } else if (pixelClass == ColorClass::DarkGrey) {
            image.data[i] = {150, 1, 1};
        } else if (pixelClass == ColorClass::Black) {
            image.data[i] = {180, 1, 1};
        } else if (pixelClass == ColorClass::White) {
            image.data[i] = {210, 1, 1};
        } else if (pixelClass == ColorClass::Red) {
            image.data[i] = {240, 1, 1};
        } else if (pixelClass == ColorClass::Pink) {
            image.data[i] = {270, 1, 1};
        } else if (pixelClass == ColorClass::Brown) {
            image.data[i] = {300, 1, 1};
        } else if (pixelClass == ColorClass::Unknown) {
            image.data[i] = {0, 0, 0};
        } else {
            throw "Unknown Color!";
        }
    }

/*
    CalibrationResult result;
    for (int row = 0; row < image.height; row++) {
        std::vector<CalibrationResult> results = Color::calibrate(calibrationColors, image, row);
        if (results.size() > 0) {
            for (int x = 0; x < 10; x++) {
                image.setPixel(x, row, {0, 1, 1});
            }
        }
    }
*/

    cout << "Converting to RGB..." << endl;
    for (int i = 0; i < pixelCount; i++) {
        PixelRGB r, g, b;
        Color::toRGB(image.data[i], r, g, b);
        data[i * 3 + 0] = r;
        data[i * 3 + 1] = g;
        data[i * 3 + 2] = b;
    }

    cout << "Writing output..." << endl;
    stbi_write_bmp(outFileName, width, height, 3, data);

    cout << "Done!" << endl;

    stbi_image_free(data);
    return 0;
}