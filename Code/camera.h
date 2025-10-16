#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <fstream>
#include <cmath>

struct Vector3 {
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z): x(x), y(y), z(z) {}

    // Addition
    Vector3 operator+(const Vector3& v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }
    
    // Subtraction
    Vector3 operator-(const Vector3& v) const {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }
    
    // Scalar multiplication
    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    // Scalar division
    Vector3 operator/(float scalar) const {
        return *this * (1.0f / scalar);
    }
    
    // Dot product
    float dot(const Vector3& v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    // Cross product
    Vector3 cross(const Vector3& v) const {
    return Vector3(
        y * v.z - z * v.y,
        z * v.x - x * v.z,
        x * v.y - y * v.x
    );
    }

    // Length
    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    
    // Normalize
    void normalize() {
        float len = length();
        *this = *this / len;
    }

};

struct Ray {
    Vector3 origin;
    Vector3 direction;

    Ray() {}
    Ray(Vector3 o, Vector3 d) : origin(o), direction(d) {}

};

class Camera {
public:
    // Camera proerties
    Vector3 location;
    Vector3 gaze;
    Vector3 cameraUp;
    float focalLength;
    float sensorWidth;
    float sensorHeight;
    int resolutionX;
    int resolutionY;

    Camera();

    bool readFromFile(const std::string& filename);

    Ray pixelToRay(float px, float py) const;

private:

    void calculateBasis();

    Vector3 right;
    Vector3 up;
    Vector3 forward;
};

#endif







