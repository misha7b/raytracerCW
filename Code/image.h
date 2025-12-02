#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <vector>
#include <algorithm> 

struct Pixel {
    unsigned char r, g, b;
    
    Pixel() : r(0), g(0), b(0) {}
    Pixel(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}

    // Pixel * scalar
    Pixel operator*(float scalar) const {
        return Pixel(
            static_cast<unsigned char>(std::clamp(r * scalar, 0.0f, 255.0f)),
            static_cast<unsigned char>(std::clamp(g * scalar, 0.0f, 255.0f)),
            static_cast<unsigned char>(std::clamp(b * scalar, 0.0f, 255.0f))
        );
    }

    // Pixel + Pixel
    Pixel operator+(const Pixel& other) const {
        return Pixel(
            static_cast<unsigned char>(std::clamp(r + other.r, 0, 255)),
            static_cast<unsigned char>(std::clamp(g + other.g, 0, 255)),
            static_cast<unsigned char>(std::clamp(b + other.b, 0, 255))
        );
    }
};

class Image {
public:
    int width;
    int height;
    
    Image(int w, int h);

    Image(const std::string& filename);

    Pixel getPixel(int x, int y) const;
    void setPixel(int x, int y, const Pixel& color);
    bool writePPM(const std::string& filename) const;
    bool readPPM(const std::string& filename);

    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    std::vector<Pixel> pixels;  
};


#endif
