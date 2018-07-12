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
};


#endif //LEGOIMAGEPROCESSING_DRAW_H
