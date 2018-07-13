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

enum class ColorClass {
    Black, Brown, Cyan, DarkGrey, DarkBlue, Green, GreyBoard, Pink, Purple, Red, White
};

struct Image {
    int width;
    int height;
    std::vector<PixelHSV> data;

    void setPixel(int x, int y, PixelHSV value) {
        data[x + y * width] = value;
    }

    const PixelHSV &getPixel(int x, int y) const {
        return data[x + y * width];
    }
};

struct PixelXY {
    int x;
    int y;
};

struct CalibrationResult {
    int start = -1;
    int end = -1;
    int row = -1;
    float score = 0;
};

struct BoardCell {
    int column;
    int row;
    CalibrationResult calibration;

    BoardCell(int column, int row, const CalibrationResult &calibration) :
            column(column), row(row), calibration(calibration) {}

    bool operator==(const BoardCell &o) const {
        return column == o.column && row == o.row;
    }

    bool operator<(const BoardCell &o) const {
        return column < o.column || (column == o.column && row < o.row);
    }
};


#endif //LEGOIMAGEPROCESSING_DATA_H
