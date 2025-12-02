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

    // Negation
    Vector3 operator-() const {
    return Vector3(-x, -y, -z);
    }
    
    // Scalar multiplication
    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    // Scalar division
    Vector3 operator/(float scalar) const {
        return *this * (1.0f / scalar);
    }

    // Component-wise vector multiplication
    Vector3 operator*(const Vector3& v) const {
        return Vector3(x * v.x, y * v.y, z * v.z);
    }

    // Component-wise vector divison
    Vector3 operator/(const Vector3& v) const {
        return Vector3(x / v.x, y / v.y, z / v.z);
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

struct Matrix3 {

    float m[3][3];

    Matrix3(float m00, float m01, float m02,
            float m10, float m11, float m12,
            float m20, float m21, float m22) {
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12;
        m[2][0] = m20; m[2][1] = m21; m[2][2] = m22;
    }

    Matrix3() {
        m[0][0] = 1; m[0][1] = 0; m[0][2] = 0;
        m[1][0] = 0; m[1][1] = 1; m[1][2] = 0;
        m[2][0] = 0; m[2][1] = 0; m[2][2] = 1;
    }

    // Matrix * Vector
    Vector3 operator*(const Vector3& v) const {
        return Vector3(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z
        );
    }

    // Matrix * Matrix
    Matrix3 operator*(const Matrix3& other) const {
        Matrix3 r;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                r.m[i][j] = m[i][0] * other.m[0][j] +
                            m[i][1] * other.m[1][j] +
                            m[i][2] * other.m[2][j];
        return r;
    }

    // Matrix transpose
    Matrix3 transpose() const {
        Matrix3 r;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                r.m[i][j] = m[j][i];
        return r;
    }

    // Matrix from Euler angles
    static Matrix3 fromEuler(float rx, float ry, float rz) {
        
        float cx = std::cos(rx), sx = std::sin(rx);
        float cy = std::cos(ry), sy = std::sin(ry);
        float cz = std::cos(rz), sz = std::sin(rz);

        Matrix3 Rx(
            1, 0, 0,
            0, cx, -sx,
            0, sx, cx
        );
        Matrix3 Ry(
            cy, 0, sy,
            0, 1, 0,
            -sy, 0, cy
        );
        Matrix3 Rz(
            cz, -sz, 0,
            sz, cz, 0,
            0, 0, 1
        );

        return Rz * Ry * Rx;
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
    float aperture;   
    float focalDistance;
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







