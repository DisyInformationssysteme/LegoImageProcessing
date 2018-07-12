//
// Created by galetzka on 7/12/18.
//

#include <vector>

#ifndef LEGOIMAGEPROCESSING_DATA_H
#define LEGOIMAGEPROCESSING_DATA_H

typedef unsigned char PixelRGB;

struct PixelHSV {
    float h; // 0 - 360
    float s; // 0 - 1
    float v; // 0 - 1
};

struct Image {
    int width;
    int height;
    std::vector<PixelHSV> data;

    void setPixel(int x, int y, PixelHSV value) {
        data[x + y * width] = value;
    }
};

struct PixelXY {
    int x;
    int y;
};

enum class ColorClass {
    Unknown, Black, Brown, Cyan, DarkGrey, DarkBlue, Green, GreyBoard, Pink, Purple, Red, White
};

struct CalibrationResult {
    int start;
    int end;
};


#endif //LEGOIMAGEPROCESSING_DATA_H
