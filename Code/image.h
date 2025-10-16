#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <vector>

struct Pixel {
    unsigned char r, g, b;
    
    Pixel() : r(0), g(0), b(0) {}
    Pixel(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}
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

private:
    std::vector<Pixel> pixels;  
};


#endif
