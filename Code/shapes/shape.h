#ifndef SHAPE_H
#define SHAPE_H
#include "../aabb.h"
#include "../camera.h" 
#include "../image.h"
#include <limits>


const float EPS_DIR = 1e-8f;
const float EPS_HIT = 1e-4f;

class Shape;

struct Material {

    Vector3 diffuse = Vector3(0.8f, 0.8f, 0.8f);  // main surface colour
    Vector3 specular = Vector3(1.0f, 1.0f, 1.0f); // highlight colour
    float shininess = 32.0f;                      // exponent 

    // Whitted-style properties
    float reflectivity = 0.0f;               
    float transparency = 0.0f;            
    float ior = 1.0f;                             // index of refraction 
    
    float roughness = 0.0f;

    std::string textureName = ""; 
    Image* texture = nullptr;     
};

struct HitInfo {
    bool hit = false;
    float t = std::numeric_limits<float>::infinity();

    Vector3 point;  // intersection point
    Vector3 normal; // intersection normal

    // texture coordinates
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