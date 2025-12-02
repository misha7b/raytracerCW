#include "image.h"
#include <iostream>
#include <fstream>

// Constructor - create blank image
Image::Image(int w, int h) : width(w), height(h) {
    pixels.resize(width * height);
}

// Constructor - load image from file
Image::Image(const std::string& filename) {
    if (!readPPM(filename)) {
        std::cerr << "Failed to load image: " << filename << std::endl;
        width = 0;
        height = 0;
    }
}

// Get pixel at (x,y)
Pixel Image::getPixel(int x, int y) const{
    return pixels[y * width + x];
}

// Set pixel at (x,y)
void Image::setPixel(int x, int y, const Pixel& colour) {
    pixels[y * width + x] = colour;
}

// Write image to PPM file
bool Image::writePPM(const std::string& filename) const {
    std::ofstream file(filename);

    file << "P3\n";
    file << width << " " << height << "\n";
    file << "255\n";

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Pixel p = getPixel(x, y);
            file << (int)p.r << " " << (int)p.g << " " << (int)p.b << " ";
        }
        file << "\n";
    }

    file.close();
    std::cout << "Image written to " << filename << std::endl;
    return true;
}

// Read image from PPM file
bool Image::readPPM(const std::string& filename) {
    std::ifstream file(filename);

    std::string magic;
    int maxVal;

    file >> magic;

    file >> width >> height;
    file >> maxVal;

    pixels.resize(width * height);

    // Read pixel data
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int r, g, b;
            file >> r >> g >> b;
            setPixel(x, y, Pixel(r, g, b));
        }
    }

    file.close();
    return true;
}
