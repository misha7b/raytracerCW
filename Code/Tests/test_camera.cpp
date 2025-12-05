#include "../camera.h"
#include <iostream>
#include <string>

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
    if (!camera.readFromFile("../ASCII/Test1.txt")) {
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
                                      camera.resolutionY / 2.0f, 0, 0, 1);
    printRayInfo("Center Pixel Ray", centerRay);

    
    const float lastPx = static_cast<float>(camera.resolutionX - 1);
    const float lastPy = static_cast<float>(camera.resolutionY - 1);

    // Test Top-Left Corner (0, 0)
    Ray tlRay = camera.pixelToRay(0.0f, 0.0f, 0, 0, 1);
    printRayInfo("Top-Left Corner (0, 0)", tlRay);

    // Test Top-Right Corner (1919, 0)
    Ray trRay = camera.pixelToRay(lastPx, 0.0f, 0, 0, 1);
    printRayInfo("Top-Right Corner (" + std::to_string((int)lastPx) + ", 0)", trRay);

    // Test Bottom-Left Corner (, 0, 1079)
    Ray blRay = camera.pixelToRay(0.0f, lastPy, 0, 0, 1);
    printRayInfo("Bottom-Left Corner (0, " + std::to_string((int)lastPy) + ")", blRay);

    // Test Bottom-Right Corner (1919, 1079)
    Ray brRay = camera.pixelToRay(lastPx, lastPy, 0, 0, 1);
    printRayInfo("Bottom-Right Corner (" + std::to_string((int)lastPx) + ", " + std::to_string((int)lastPy) + ")", brRay);

    return 0;
}