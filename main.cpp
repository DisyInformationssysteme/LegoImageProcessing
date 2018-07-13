#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include <map>
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
    char const *inFileName = "/home/galetzka/dev/LegoImageProcessing/test/extract7.jpg";
    char const *inMaskName = "/home/galetzka/dev/LegoImageProcessing/mask_greyscale.png";
    char const *outFileName = "/home/galetzka/dev/LegoImageProcessing/test/out.bmp";
    int width, height, channels;
    cout << "Reading image..." << endl;
    PixelRGB *data = stbi_load(inFileName, &width, &height, &channels, 3);
    int pixelCount = width * height;
    if (data == nullptr) {
        cout << "No file found / error processing image" << endl;
        return -1;
    }
    cout << "  Width: " << width << ", Height: " << height << ", Channels: " << channels << endl;

    cout << "Converting to HSV..." << endl;
    vector<PixelHSV> hsvData(pixelCount);
    for (int i = 0; i < pixelCount; i++) {
        PixelRGB r = data[i * 3 + 0];
        PixelRGB g = data[i * 3 + 1];
        PixelRGB b = data[i * 3 + 2];
        hsvData[i] = Color::toHSV(r, g, b);
    }
    Image image = {width, height, hsvData};

    cout << "Reading mask..." << endl;
    int maskWidth, maskHeight, maskChannels;
    PixelRGB* maskData = stbi_load(inMaskName, &maskWidth, &maskHeight, &maskChannels, 1);
    cout << "  Width: " << maskWidth << ", Height: " << maskHeight << ", Channels: " << maskChannels << endl;

/*
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
        if (pixelClass.size() == 0) {
            continue;
        }
        if (pixelClass[0] == ColorClass::Cyan) {
            image.data[i] = {0, 1, 1};
        } else if (pixelClass[0] == ColorClass::DarkBlue) {
            image.data[i] = {30, 1, 1};
        } else if (pixelClass[0] == ColorClass::GreyBoard) {
            image.data[i] = {60, 1, 1};
        } else if (pixelClass[0] == ColorClass::Purple) {
            image.data[i] = {90, 1, 1};
        } else if (pixelClass[0] == ColorClass::Green) {
            image.data[i] = {120, 1, 1};
        } else if (pixelClass[0] == ColorClass::DarkGrey) {
            image.data[i] = {330, 1, 1};
        } else if (pixelClass[0] == ColorClass::Black) {
            image.data[i] = {180, 1, 1};
        } else if (pixelClass[0] == ColorClass::White) {
            image.data[i] = {210, 1, 1};
        } else if (pixelClass[0] == ColorClass::Red) {
            image.data[i] = {240, 1, 1};
        } else if (pixelClass[0] == ColorClass::Pink) {
            image.data[i] = {270, 1, 1};
        } else if (pixelClass[0] == ColorClass::Brown) {
            image.data[i] = {300, 1, 1};
        }
    }
*/


    int cellBlockWidth = 24;
    int cellBlockHeight = 23;
    float blockWidth = 33.2;
    int lastMatchedRow = -1;
    map<BoardCell, float> foundStrips;

    cout << "Finding calibration strips..." << endl;
    for (int row = 0; row < image.height; row += blockWidth / 4) {
        std::vector<CalibrationResult> calibrations = Color::calibrate(calibrationColors, image, row, blockWidth, 0, image.width);
        int rowDistance = row - lastMatchedRow;
        if (!calibrations.empty()) {
            for (auto &calibration : calibrations) {
                int cellColumn = calibration.start / (cellBlockWidth * blockWidth);
                int cellRow = row / (cellBlockHeight * blockWidth);
                BoardCell cell(cellColumn, cellRow, calibration);

                auto iter = foundStrips.find(cell);
                if (iter == foundStrips.end()) {
                    foundStrips[cell] = calibration.score;
                } else if (iter->second < calibration.score) {
                    foundStrips.erase(iter);
                    foundStrips[cell] = calibration.score;
                }
            }
            lastMatchedRow = row;
        } else if (lastMatchedRow > 0 && rowDistance > blockWidth && rowDistance < blockWidth * 4) {
            // optimization to jump ahead a lot if we found a strip a few pixels back
            row += blockWidth * 20;
        }
    }

    for (const auto& strip : foundStrips) {
        auto calibration = strip.first.calibration;
        //TODO: fine-tune calibration
        cout << "   Found strip pixel (" << calibration.start << "-" << calibration.end << ", " << calibration.row << ")" << endl;

        // searching for upper bound of strip
        vector<ColorClass> startColor;
        startColor.push_back(calibrationColors.front());
        CalibrationResult upperBound;
        for (int i = -blockWidth; i < blockWidth; i++) {
            int row = calibration.row + i;
            if (row < 0 || row >= image.height) {
                continue;
            }
            std::vector<CalibrationResult> calibrations = Color::calibrate(startColor, image, row, blockWidth, calibration.start,
                                                                           calibration.start + 1);
            if (!calibrations.empty()) {
                upperBound = calibrations.front();
                break;
            }
        }
        for (int x = upperBound.start; x < upperBound.end; x++) {
            image.setPixel(x, upperBound.row - 1, {0, 1, 1});
            image.setPixel(x, upperBound.row, {0, 1, 1});
            image.setPixel(x, upperBound.row + 1, {0, 1, 1});
        }

        int startX = upperBound.start - blockWidth * 2.5;
        int startY = upperBound.row;
        for (int col = 0; col < cellBlockWidth * 2 - 1; col++) {
            for (int row = 0; row < cellBlockHeight * 2 - 1; row++) {
                PixelXY upperLeft = {startX + col * blockWidth / 2, startY + row * blockWidth / 2};
                PixelXY lowerRight = {upperLeft.x + blockWidth / 2, upperLeft.y + blockWidth / 2};

                int maskOffsetX = cellBlockWidth * 2;
                int maskOffsetY = strip.first.row * cellBlockHeight * 2;
                auto maskValue = maskData[maskOffsetX + col + (row + maskOffsetY) * maskWidth];
                if (maskValue == 0) {
                    Draw::fillRectangle(image, upperLeft, lowerRight, {0, 0, 0});
                    continue;
                }
                map<ColorClass, int> colorCount;
                for (int y = upperLeft.y; y <= lowerRight.y; y++) {
                    for (int x = upperLeft.x; x <= lowerRight.x; x++) {
                        for (auto& colorClass : Color::classify(image.getPixel(x, y))) {
                            colorCount[colorClass]++;
                        }
                    }
                }
                pair<ColorClass , int> bestResult = *colorCount.begin();
                for (auto pair: colorCount) {
                    if (pair.second > bestResult.second) {
                        bestResult = pair;
                    }
                }
                Draw::fillRectangle(image, upperLeft, lowerRight, bestResult.first);
            }
        }

    }


    cout << "Converting to RGB..." << endl;
    for (int i = 0; i < pixelCount; i++) {
        PixelRGB r, g, b;
        Color::toRGB(image.data[i], r, g, b);
        data[i * 3 + 0] = r;
        data[i * 3 + 1] = g;
        data[i * 3 + 2] = b;
    }

    cout << "Writing output..." << endl;
    stbi_write_bmp(outFileName, image.width, image.height, 3, data);

    cout << "Done!" << endl;

    stbi_image_free(data);
    return 0;
}