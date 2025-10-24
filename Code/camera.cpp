#include "camera.h"
#include <iostream>
#include <cmath>

// Constructor
Camera::Camera() {
}

// Read scene from file
bool Camera::readFromFile(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }

    std::string label;
    bool inCameraBlock = false;

    while (file >> label) {

        if (label == "BEGIN_CAMERA") {
            inCameraBlock = true;
            continue;
        }

        if (label == "END_CAMERA") {
            break;
        }

        if (inCameraBlock) {
            if (label == "location") {
                file >> location.x >> location.y >> location.z;
            } else if (label == "gaze") {
                file >> gaze.x >> gaze.y >> gaze.z;
            } else if (label == "up") {
                file >> cameraUp.x >> cameraUp.y >> cameraUp.z;
            } else if (label == "focal_length") {
                file >> focalLength;
            } else if (label == "sensor_size") {
                file >> sensorWidth >> sensorHeight;
            } else if (label == "resolution") {
                file >> resolutionX >> resolutionY;
            }
        }
    }

    file.close();
    calculateBasis();

    std::cout << "Camera loaded successfully" << std::endl;
    return true;
}

// Calculate camera orthonormal basis vectors (right, up, forward)
void Camera::calculateBasis() {

    // forward is gaze direction
    forward = gaze;
    forward.normalize();

    // right = forward x cameraUp
    right = forward.cross(cameraUp);
    right.normalize();

    // up = right x forward
    up = right.cross(forward);
    up.normalize();

}

// Convert pixel coordinates to ray
// (px, py) range from (0, 0) to (resolutionX-1, resolutionY-1).
Ray Camera::pixelToRay(float px, float py) const{

    float u_normalized = (px + 0.5f) / resolutionX;
    float v_normalized = (py + 0.5f) / resolutionY;

    // Normalized coordinates in [-0.5, 0.5]
    float u = u_normalized - 0.5f;
    float v = 0.5f - v_normalized;

    // Scale by sensor size
    float aspectRatio = (float)resolutionX / (float)resolutionY;
    float worldX_offset = u * sensorWidth;
    float worldY_offset = v * (sensorWidth / aspectRatio);
    

    Vector3 imagePlaneCenter = location + (forward * focalLength);
    Vector3 imagePoint = imagePlaneCenter + (right * worldX_offset) + (up * worldY_offset);

    Vector3 direction = imagePoint - location;
    direction.normalize();

    return Ray(location, direction);

}
