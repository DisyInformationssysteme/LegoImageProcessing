//
// Created by galetzka on 7/12/18.
//

#include <cstdlib>
#include <cmath>
#include <algorithm>
#include "Data.h"

#ifndef LEGOIMAGEPROCESSING_COLOR_H
#define LEGOIMAGEPROCESSING_COLOR_H

class Color {
public:

    static std::vector<CalibrationResult>
    calibrate(const std::vector<ColorClass> &colors, const Image &image, int row, int blockWidth, int minX, int maxX) {
        int windowWidth = blockWidth * colors.size();
        std::vector<CalibrationResult> results;

        CalibrationResult lastResult;
        lastResult.row = row;
        float lastScore;
        int endX = std::min(maxX, image.width - windowWidth);
        int stepWidth = (endX - minX) <= blockWidth ? 1 :  blockWidth / 4;
        for (int startX = minX; startX < endX; startX += stepWidth) {
            std::vector<int> matches(colors.size());
            float score = matchColorWindow(colors, image, row, blockWidth, matches, startX);
            int distanceToLast = startX - lastResult.start;
            if (score > lastScore) {
                if (lastResult.start >= 0 && distanceToLast < blockWidth) {
                    results.back().start = startX;
                    results.back().end = startX + windowWidth;
                    results.back().score = score;
                } else {
                    lastResult.start = startX;
                    lastResult.end = startX + windowWidth;
                    lastResult.score = score;
                    results.push_back(lastResult);
                }
            } else if (lastResult.start >= 0 && distanceToLast > blockWidth && distanceToLast < windowWidth) {
                startX += windowWidth;
            }
            lastScore = score;
        }

        return results;
    }

    static float matchColorWindow(const std::vector<ColorClass> &colors, const Image &image, int row, int blockWidth,
                                  std::vector<int> &matches, int startX) {
        float threshold = 0.5;
        float matchScore = 0;
        for (int i = 0; i < colors.size(); i++) {
            auto &requiredColor = colors[i];
            for (int offset = 0; offset < blockWidth; offset++) {
                int x = startX + offset + i * blockWidth;
                auto pixelColors = classify(image.getPixel(x, row));
                for (auto &pixelColor : pixelColors) {
                    if (pixelColor == requiredColor) {
                        matches[i]++;
                    }
                }
            }
            float score = matches[i] / (float) blockWidth;
            if (score < threshold) {
                return 0;
            }
            matchScore += score;
        }
        return matchScore / colors.size();
    }


    static std::vector<ColorClass> classify(const PixelHSV &color) {
        // TODO: move values to config file

        PixelHSV blackMin = {0, 0, 0};
        PixelHSV blackMax = {360, 1, 0.2};

        PixelHSV brownMin = {0, 0.2, 0.1};
        PixelHSV brownMax = {30, 0.9, 0.6};

        PixelHSV cyanMin = {170, 0.4, 0.2};
        PixelHSV cyanMax = {200, 1, 1};

        PixelHSV darkGreyMin = {0, 0, 0.10};
        PixelHSV darkGreyMax = {360, 0.25, 0.5};

        PixelHSV darkBlueMin = {190, 0.4, 0.3};
        PixelHSV darkBlueMax = {230, 1, 1};

        PixelHSV greenMin = {120, 0.4, 0.2};
        PixelHSV greenMax = {170, 1, 1};

        PixelHSV greyBoardMin = {0, 0, 0.2};
        PixelHSV greyBoardMax = {360, 0.25, 0.8};

        PixelHSV pinkMin = {310, 0.15, 0.5};
        PixelHSV pinkMax = {350, 0.41, 1};

        PixelHSV purpleMin = {240, 0.2, 0.2};
        PixelHSV purpleMax = {270, 0.8, 0.8};

        PixelHSV redMin = {210, 0.5, 0.5};
        PixelHSV redMax = {20, 1, 1};

        PixelHSV whiteMin = {0, 0, 0.7};
        PixelHSV whiteMax = {360, 0.2, 1};

        std::vector<ColorClass> classes;
        if (inRange(color, blackMin, blackMax)) {
            classes.push_back(ColorClass::Black);
        }
        if (inRange(color, brownMin, brownMax)) {
            classes.push_back(ColorClass::Brown);
        }
        if (inRange(color, cyanMin, cyanMax)) {
            classes.push_back(ColorClass::Cyan);
        }
        if (inRange(color, darkGreyMin, darkGreyMax)) {
            classes.push_back(ColorClass::DarkGrey);
        }
        if (inRange(color, darkBlueMin, darkBlueMax)) {
            classes.push_back(ColorClass::DarkBlue);
        }
        if (inRange(color, greenMin, greenMax)) {
            classes.push_back(ColorClass::Green);
        }
        if (inRange(color, greyBoardMin, greyBoardMax)) {
            classes.push_back(ColorClass::GreyBoard);
        }
        if (inRange(color, pinkMin, pinkMax)) {
            classes.push_back(ColorClass::Pink);
        }
        if (inRange(color, purpleMin, purpleMax)) {
            classes.push_back(ColorClass::Purple);
        }
        if (inRange(color, redMin, redMax)) {
            classes.push_back(ColorClass::Red);
        }
        if (inRange(color, whiteMin, whiteMax)) {
            classes.push_back(ColorClass::White);
        }

        return classes;
    }

    static bool inRange(const PixelHSV &val, const PixelHSV &min, const PixelHSV &max) {
        return (max.h >= min.h ? (val.h >= min.h && val.h <= max.h) : (val.h >= min.h || val.h <= max.h))
               && (val.s >= min.s && val.s <= max.s) && (val.v >= min.v && val.v <= max.v);
    }

    static PixelHSV toHSV(int r, int g, int b) {
        float fR = r / 255.f;
        float fG = g / 255.f;
        float fB = b / 255.f;

        float fCMax = std::max(std::max(fR, fG), fB);
        float fCMin = std::min(std::min(fR, fG), fB);
        float fDelta = fCMax - fCMin;

        float fH, fS, fV;
        if (fDelta > 0) {
            if (fCMax == fR) {
                fH = 60 * (fmod(((fG - fB) / fDelta), 6));
            } else if (fCMax == fG) {
                fH = 60 * (((fB - fR) / fDelta) + 2);
            } else if (fCMax == fB) {
                fH = 60 * (((fR - fG) / fDelta) + 4);
            } else {
                fH = 0;
            }

            if (fCMax > 0) {
                fS = fDelta / fCMax;
            } else {
                fS = 0;
            }

            fV = fCMax;
        } else {
            fH = 0;
            fS = 0;
            fV = fCMax;
        }

        if (fH < 0) {
            fH = 360 + fH;
        }
        return {fH, fS, fV};
    }

    static void toRGB(const PixelHSV &pixel, PixelRGB &r, PixelRGB &g, PixelRGB &b) {
        float fC = pixel.v * pixel.s; // Chroma
        float fHPrime = fmod(pixel.h / 60.0, 6);
        float fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
        float fM = pixel.v - fC;

        float fR, fG, fB;

        if (0 <= fHPrime && fHPrime < 1) {
            fR = fC;
            fG = fX;
            fB = 0;
        } else if (1 <= fHPrime && fHPrime < 2) {
            fR = fX;
            fG = fC;
            fB = 0;
        } else if (2 <= fHPrime && fHPrime < 3) {
            fR = 0;
            fG = fC;
            fB = fX;
        } else if (3 <= fHPrime && fHPrime < 4) {
            fR = 0;
            fG = fX;
            fB = fC;
        } else if (4 <= fHPrime && fHPrime < 5) {
            fR = fX;
            fG = 0;
            fB = fC;
        } else if (5 <= fHPrime && fHPrime < 6) {
            fR = fC;
            fG = 0;
            fB = fX;
        } else {
            fR = 0;
            fG = 0;
            fB = 0;
        }

        fR += fM;
        fG += fM;
        fB += fM;

        r = (PixelRGB) (fR * 255);
        g = (PixelRGB) (fG * 255);
        b = (PixelRGB) (fB * 255);
    }
};


#endif //LEGOIMAGEPROCESSING_COLOR_H
