#ifndef SHAPE_H
#define SHAPE_H

#include <limits>
#include "../camera.h" 

const float EPS_DIR = 1e-8f;
const float EPS_HIT = 1e-4f;

struct HitInfo {
    bool hit = false;          
    float t = std::numeric_limits<float>::infinity(); 
    Vector3 point;             
    Vector3 normal;            
};

class Shape {
public:
    virtual ~Shape() =  default;

    virtual bool intersect(const Ray& ray, HitInfo& hit) const = 0;

};


#endif