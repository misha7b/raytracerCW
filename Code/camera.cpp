#include "camera.h"
#include <iostream>
#include <cmath>
#include <cstdlib>

#include "utils.h"

static void sampleUnitDisk(float& x, float& y, int gridX, int gridY, int gridSize) {
    float cellSize = 1.0f / static_cast<float>(gridSize);

    // Jitter within grid
    float r1 = (gridX * cellSize) + (randomFloat() * cellSize);
    float r2 = (gridY * cellSize) + (randomFloat() * cellSize);

    // Map [0,1] to [-1,1] 
    float sx = 2.0f * r1 - 1.0f;
    float sy = 2.0f * r2 - 1.0f;

    // Edge case, avoid division by 0
    if (sx == 0 && sy == 0) { x = 0; y = 0; return; }

    // Map square to disk
    float r, theta;
    if (sx * sx > sy * sy) {
        r = sx;
        theta = (M_PI / 4.0f) * (sy / sx);
    } else {
        r = sy;
        theta = (M_PI / 2.0f) - (M_PI / 4.0f) * (sx / sy);
    }

    x = r * cos(theta);
    y = r * sin(theta);
}

// Constructor
Camera::Camera() 
    : location(0,0,0), gaze(0,0,-1), cameraUp(0,1,0), velocity(0,0,0),
      focalLength(1.0f), sensorWidth(1.0f), sensorHeight(1.0f),
      aperture(0.0f), focalDistance(1.0f), 
      resolutionX(0), resolutionY(0) 
{
}

// Read Camera Data
bool Camera::readFromFile(const std::string& filename) {

    std::cout << "Loading scene..." << std::endl;

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
            else if (label == "aperture") {
                file >> aperture;
            } else if (label == "focal_distance") {
                file >> focalDistance;
            }
            else if (label == "velocity") {
                file >> velocity.x >> velocity.y >> velocity.z; 
            }
        }
    }

    file.close();
    
    calculateBasis();

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
Ray Camera::pixelToRay(float px, float py, int sX, int sY, int gridSide) const {

    float u_normalized = (px + 0.5f) / resolutionX;
    float v_normalized = (py + 0.5f) / resolutionY;

    // Normalized coordinates in [-0.5, 0.5]
    float u = u_normalized - 0.5f;
    float v = 0.5f - v_normalized;

    // Scale by sensor size
    float aspectRatio = (float)resolutionX / (float)resolutionY;
    float worldX_offset = u * sensorWidth;
    float worldY_offset = v * (sensorWidth / aspectRatio);

    Vector3 currentPos = location;

    // Motion Blur
    if (std::abs(velocity.x) > 1e-6 || std::abs(velocity.y) > 1e-6 || std::abs(velocity.z) > 1e-6) {
        float time = randomFloat(); // random time between 0.0 and 1.0
        currentPos = location + (velocity * time);
    }

    // Compute point on image plane in world space
    Vector3 imagePlaneCenter = currentPos + (forward * focalLength);
    Vector3 targetPoint = imagePlaneCenter + (right * worldX_offset) + (up * worldY_offset);

    Vector3 direction = targetPoint - currentPos;
    direction.normalize();

    // Depth of Field
    // If aperture > 0, sample points on lens instead of pinhole
    if (aperture > 0.0f) {
        
        float t = focalDistance / forward.dot(direction);
        Vector3 focalPoint = currentPos + (direction * t);

        // Random point on lens
        float lensRadius = aperture * 0.5f;
        float diskX, diskY;
        sampleUnitDisk(diskX, diskY, sX, sY, gridSide);
        
        Vector3 lensOffset = (right * diskX * lensRadius) + (up * diskY * lensRadius);
        Vector3 lensOrigin = currentPos + lensOffset;

        Vector3 lensDir = focalPoint - lensOrigin;
        lensDir.normalize();

        return Ray(lensOrigin, lensDir);
    }

    // Default pinhole camera
    return Ray(currentPos, direction);


}
