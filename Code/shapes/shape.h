#ifndef SHAPE_H
#define SHAPE_H

#include <string>
#include <limits>
#include "../maths.h"  
#include "../aabb.h"

class Image; 
class Shape;

const float EPS_DIR = 1e-8f;
const float EPS_HIT = 1e-4f;

// Material info
struct Material {

    Vector3 diffuse = Vector3(0.8f, 0.8f, 0.8f);  // base colour
    Vector3 specular = Vector3(1.0f, 1.0f, 1.0f); // highlight colour
    float shininess = 32.0f;                      // exponent 

    // Whitted-style properties
    float reflectivity = 0.0f;                    // 0.0 (matte) to 1.0 (mirror)
    float transparency = 0.0f;                    // 0.0 (opaque) to 1.0 (glass)
    float ior = 1.0f;                             // index of refraction

    float roughness = 0.0f;

    std::string textureName = ""; 
    Image* texture = nullptr;     
};

// Hit info
struct HitInfo {
    bool hit = false;
    float t = std::numeric_limits<float>::infinity();

    Vector3 point;  // intersection point
    Vector3 normal; // intersection normal

    // Texture coordinates
    float u = 0.0f; 
    float v = 0.0f;

    Shape* shape = nullptr;
};

class Shape {
public:
    Material material;

    virtual ~Shape() = default;

    virtual bool intersect(const Ray& ray, HitInfo& hit) const = 0;
    virtual AABB bounds() const = 0;
    virtual Vector3 centroid() const = 0;
};

#endif