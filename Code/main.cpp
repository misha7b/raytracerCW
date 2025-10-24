#include "camera.h"
#include "image.h"
#include "scene.h"
#include "shapes/cube.cpp"
#include "shapes/sphere.cpp"
#include "shapes/plane.cpp"
#include <vector>
#include <iostream>

int main() {
    Camera cam;
    std::vector<Shape*> shapes;
    
    if (!loadScene("../ASCII/test.txt", cam, shapes)) {
        std::cerr << "Failed to load scene!\n";
        return 1;
    }

    Image img(cam.resolutionX, cam.resolutionY);

    for (int y = 0; y < cam.resolutionY; ++y) {
        for (int x = 0; x < cam.resolutionX; ++x) {

            Ray r = cam.pixelToRay(x, y);
            HitInfo closest;
            Shape* hitShape = nullptr;

            for (auto s : shapes) {
                HitInfo temp;
                if (s->intersect(r, temp) && temp.t < closest.t) {
                    closest = temp;
                    hitShape = s;
                }
            }

            Pixel colour(0, 0, 0);
            if (closest.hit) {
                if (dynamic_cast<Sphere*>(hitShape))
                    colour = Pixel(255, 0, 0);
                else if (dynamic_cast<Cube*>(hitShape))
                    colour = Pixel(0, 255, 0);
                else if (dynamic_cast<Plane*>(hitShape))
                    colour = Pixel(0, 0, 255);
            }

            img.setPixel(x, y, colour);
        }
    }

    img.writePPM("../Output/module2_test.ppm");

    for (auto s : shapes) delete s;

    return 0;
}

