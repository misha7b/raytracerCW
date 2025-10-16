#include "../camera.h"
#include <iostream>

void printRayInfo(const std::string& label, const Ray& ray) {
    std::cout << label << ":\n";
    std::cout << "  Origin:    (" << ray.origin.x << ", " 
              << ray.origin.y << ", " << ray.origin.z << ")\n";
    std::cout << "  Direction: (" << ray.direction.x << ", " 
              << ray.direction.y << ", " << ray.direction.z << ")\n";
}

int main()
{
    Camera camera;

    // Load camera from exported file
    if (!camera.readFromFile("../ASCII/test.txt")) {
        std::cerr << "Failed to load camera!" << std::endl;
        return 1;
    }

    std::cout << "\nCamera properties:\n";
    std::cout << "  Location:    " << camera.location.x << ", "
              << camera.location.y << ", " << camera.location.z << '\n';
    std::cout << "  Gaze:        " << camera.gaze.x << ", "
              << camera.gaze.y << ", " << camera.gaze.z << '\n';
    std::cout << "  Up:          " << camera.cameraUp.x << "\n";
    std::cout << "  Focal Length:" << ' ' << camera.focalLength << " mm\n";
    std::cout << "  Sensor:      " << camera.sensorWidth << " x "
              << camera.sensorHeight << " mm\n";
    std::cout << "  Resolution:  " << camera.resolutionX << " x "
              << camera.resolutionY << " px\n";

    std::cout << "\n";

    Ray centerRay = camera.pixelToRay(camera.resolutionX / 2.0f,
                                      camera.resolutionY / 2.0f);
    printRayInfo("Center Pixel Ray", centerRay);

    
    const float lastPx = static_cast<float>(camera.resolutionX - 1);
    const float lastPy = static_cast<float>(camera.resolutionY - 1);

    // Test Top-Left Corner (0, 0)
    Ray tlRay = camera.pixelToRay(0.0f, 0.0f);
    printRayInfo("Top-Left Corner (0, 0)", tlRay);

    // Test Top-Right Corner (1919, 0)
    Ray trRay = camera.pixelToRay(lastPx, 0.0f);
    printRayInfo("Top-Right Corner (" + std::to_string((int)lastPx) + ", 0)", trRay);

    // Test Bottom-Left Corner (, 0, 1079)
    Ray blRay = camera.pixelToRay(0.0f, lastPy);
    printRayInfo("Bottom-Left Corner (0, " + std::to_string((int)lastPy) + ")", blRay);

    // Test Bottom-Right Corner (1919, 1079)
    Ray brRay = camera.pixelToRay(lastPx, lastPy);
    printRayInfo("Bottom-Right Corner (" + std::to_string((int)lastPx) + ", " + std::to_string((int)lastPy) + ")", brRay);


    std::ofstream out("rays.txt");

    int nx = 10, ny = 5;
    for (int j = 0; j < ny; ++j) {
        for (int i = 0; i < nx; ++i) {
            float px = (i + 0.5f) * (camera.resolutionX / float(nx));
            float py = (j + 0.5f) * (camera.resolutionY / float(ny));

            Ray r = camera.pixelToRay(px, py);

            out << r.origin.x << " " << r.origin.y << " " << r.origin.z << " "
                << r.direction.x << " " << r.direction.y << " " << r.direction.z << "\n";
        }
    }

    out.close();



    return 0;
}