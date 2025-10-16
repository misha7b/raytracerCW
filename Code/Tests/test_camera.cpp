#include "../camera.h"
#include <iostream>

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
    std::cout << "  Focal Length:" << ' ' << camera.focalLength << " mm\n";
    std::cout << "  Sensor:      " << camera.sensorWidth << " x "
              << camera.sensorHeight << " mm\n";
    std::cout << "  Resolution:  " << camera.resolutionX << " x "
              << camera.resolutionY << " px\n";

    // Test pixelToRay
    std::cout << "\nTesting pixelToRay():\n";
    Ray centerRay = camera.pixelToRay(camera.resolutionX / 2.0f,
                                       camera.resolutionY / 2.0f);

    std::cout << "Center pixel ray:\n";
    std::cout << "  Origin:    " << centerRay.origin.x << ", "
              << centerRay.origin.y << ", " << centerRay.origin.z << '\n';
    std::cout << "  Direction: " << centerRay.direction.x << ", "
              << centerRay.direction.y << ", " << centerRay.direction.z << '\n';

    // Test corner pixel
    Ray cornerRay = camera.pixelToRay(0, 0);
    std::cout << "Corner (0,0) ray direction: "
              << cornerRay.direction.x << ", "
              << cornerRay.direction.y << ", "
              << cornerRay.direction.z << '\n';

    return 0;
}