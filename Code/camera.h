#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <fstream>

#include "maths.h"

class Camera {
public:
    // Camera properties
    Vector3 location;
    Vector3 gaze;
    Vector3 cameraUp;
    Vector3 velocity;

    float focalLength;
    float sensorWidth;
    float sensorHeight;
    float aperture;   
    float focalDistance;

    int resolutionX;
    int resolutionY;

    Camera();

    bool readFromFile(const std::string& filename);

    Ray pixelToRay(float px, float py, int sX, int sY, int gridSide) const;

private:

    void calculateBasis();

    Vector3 right;
    Vector3 up;
    Vector3 forward;
};

#endif







