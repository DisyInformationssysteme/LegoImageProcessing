//
// Created by galetzka on 7/12/18.
//

#ifndef LEGOIMAGEPROCESSING_DRAW_H
#define LEGOIMAGEPROCESSING_DRAW_H


#include "Data.h"

class Draw {
public:
    static void rectangle(Image& image, PixelXY upperLeft, PixelXY lowerRight, PixelHSV color) {
        for (int x = upperLeft.x; x <= lowerRight.x; x++) {
            image.setPixel(x, upperLeft.y, color);
            image.setPixel(x, lowerRight.y, color);
        }
        for (int y = upperLeft.y; y <= lowerRight.y; y++) {
            image.setPixel(upperLeft.x, y, color);
            image.setPixel(lowerRight.x, y, color);
        }
    }

    static void fillRectangle(Image& image, PixelXY upperLeft, PixelXY lowerRight, PixelHSV color) {
        for (int y = upperLeft.y; y <= lowerRight.y; y++) {
            for (int x = upperLeft.x; x <= lowerRight.x; x++) {
                image.setPixel(x, y, color);
            }
        }
    }

    static void fillRectangle(Image& image, PixelXY upperLeft, PixelXY lowerRight, ColorClass colorClass) {
        PixelHSV color;
        if (colorClass == ColorClass::Cyan) {
            color = {180, 1, 1};
        } else if (colorClass == ColorClass::DarkBlue) {
            color = {240, 1, 1};
        } else if (colorClass == ColorClass::GreyBoard) {
            color = {0, 0, 0.5};
        } else if (colorClass == ColorClass::Purple) {
            color = {290, 1, 0.3};
        } else if (colorClass == ColorClass::Green) {
            color = {110, 1, 1};
        } else if (colorClass == ColorClass::DarkGrey) {
            color = {0, 0, 0.2};
        } else if (colorClass == ColorClass::Black) {
            color = {0, 0, 0};
        } else if (colorClass == ColorClass::White) {
            color = {0, 0, 1};
        } else if (colorClass == ColorClass::Red) {
            color = {0, 1, 1};
        } else if (colorClass == ColorClass::Pink) {
            color = {300, 0.6, 1};
        } else if (colorClass == ColorClass::Brown) {
            color = {0, 1, 0.35};
        }
        fillRectangle(image, upperLeft, lowerRight, color);
    }
};


#endif //LEGOIMAGEPROCESSING_DRAW_H
