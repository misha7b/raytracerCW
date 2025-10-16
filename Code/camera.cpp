#include "camera.h"
#include <iostream>
#include <cmath>

// Constructor
Camera::Camera() {
}

// Read camera data from file
bool Camera::readFromFile(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }

    std::string label;

    while (file >> label) {
        if (label == "BEGIN_CAMERA")
            break;
    }

    if (file.eof()) {
        std::cerr << "Error: BEGIN_CAMERA not found in " << filename << std::endl;
        return false;
    }

    std::string name;
    file >> label >> name;                           
    file >> label >> location.x >> location.y >> location.z;   
    file >> label >> gaze.x >> gaze.y >> gaze.z;            
    file >> label >> focalLength;                           
    file >> label >> sensorWidth;                              
    file >> label >> sensorHeight;                            
    file >> label >> resolutionX;                               
    file >> label >> resolutionY;                               

    while (file >> label) {
        if (label == "END_CAMERA")
            break;
    }
    
    file.close();

    // Calculate camera basis vectors
    calculateBasis();

    std::cout << "Camera loaded" << std::endl;
    std::cout << "Location: " << location.x << ", " << location.y << ", " << location.z << std::endl;

    return true;

}

// Calculate camera orthonormal basis vectors (right, up, forward)
void Camera::calculateBasis() {

    // forward is gaze direction
    forward = gaze;
    forward.normalize();

    Vector3 worldUp(0, 0, 1);

    // right = forward x worldUp
    right = forward.cross(worldUp);
    right.normalize();

    // up = right x forward
    up = right.cross(forward);
    up.normalize();

}

// Convert pixel coordinates to ray
Ray Camera::pixelToRay(float px, float py){

    // Maps pixel coordinates to [-0.5, 0.5]
    float u = px / resolutionX;
    float v = py / resolutionY;
    u = u - 0.5f;
    v = v - 0.5f;

    // Scale by sensor size
    float worldX = u * sensorWidth;
    float worldY = v * sensorHeight;

    Vector3 imagePoint = location + forward * focalLength + right * worldX + up * worldY;

    Vector3 direction = imagePoint - location;
    direction.normalize();

    return Ray(location, direction);

}
