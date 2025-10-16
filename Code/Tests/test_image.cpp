#include "../image.h"
#include <iostream>

int main() {
    // Create gradient image
    Image img(100, 100);
    
    for (int y = 0; y < 100; y++) {
        for (int x = 0; x < 100; x++) {
            unsigned char r = (x * 255) / 100;
            unsigned char g = (y * 255) / 100;
            unsigned char b = 128;
            img.setPixel(x, y, Pixel(r, g, b));
        }
    }
    
    img.writePPM("../Output/gradient.ppm");
    
    //  Create RGB stripes
    Image stripes(300, 100);
    
    for (int y = 0; y < 100; y++) {
        for (int x = 0; x < 100; x++) {
            stripes.setPixel(x, y, Pixel(255, 0, 0));  // Red
        }
        for (int x = 100; x < 200; x++) {
            stripes.setPixel(x, y, Pixel(0, 255, 0));  // Green
        }
        for (int x = 200; x < 300; x++) {
            stripes.setPixel(x, y, Pixel(0, 0, 255));  // Blue
        }
    }
    
    stripes.writePPM("../Output/stripes.ppm");
    
    // Read and write 
    Image loaded("../Output/gradient.ppm");

    int blockSize = 10; 
    for (int y = 0; y < loaded.height; y++) {
        for (int x = 0; x < loaded.width; x++) {
            bool blackSquare = ((x / blockSize) + (y / blockSize)) % 2 == 0;
            if (blackSquare) {
                loaded.setPixel(x, y, Pixel(0, 0, 0));
            }
           
        }
    }
    loaded.writePPM("../Output/gradient_copy.ppm");
    
    
    return 0;
}