#include "../image.h"
#include <iostream>

int main() {
    // Create gradient image
    std::cout << "Creating 100x100 gradient image..." << std::endl;
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
    std::cout << "Creating 300x100 RGB stripes..." << std::endl;
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
    std::cout << "Testing read/write..." << std::endl;
    Image loaded("../Output/gradient.ppm");
    loaded.writePPM("../Output/gradient_copy.ppm");
    
    
    return 0;
}