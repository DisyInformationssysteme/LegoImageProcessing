#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include <vector>
#include "lib/stb_image.h"
#include "lib/stb_image_write.h"
#include "Color.h"

using namespace std;

int main() {
    char const* inFileName = "/home/galetzka/dev/LegoImageProcessing/test/test.jpg";
    char const* outFileName = "/home/galetzka/dev/LegoImageProcessing/test/out.bmp";
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

    cout << "Moving window calibration..." << endl;


    cout << "Converting to RGB..." << endl;
    for (int i = 0; i < pixelCount; i++) {
        PixelRGB r, g, b;
        Color::toRGB(hsvData[i], r, g, b);
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