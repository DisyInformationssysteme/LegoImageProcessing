//
// Created by galetzka on 7/12/18.
//

#include <cstdlib>
#include <cmath>
#include <algorithm>

#ifndef LEGOIMAGEPROCESSING_COLOR_H
#define LEGOIMAGEPROCESSING_COLOR_H

typedef unsigned char PixelRGB;

struct PixelHSV {
    float h; // 0 - 360
    float s; // 0 - 1
    float v; // 0 - 1
};

class Color {
public:
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

    static void toRGB(PixelHSV pixel, PixelRGB &r, PixelRGB &g, PixelRGB &b) {
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
