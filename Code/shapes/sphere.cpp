#include "shape.h"
#include <cmath>
#include <algorithm> 

class Sphere : public Shape {
public:
    Vector3 center;
    float radius;

    Sphere(const Vector3& c, float r) : center(c), radius(r) {}

    bool intersect(const Ray& ray, HitInfo& hit) const override {

        Vector3 oc = ray.origin - center;

        float a = ray.direction.dot(ray.direction);  // d (dot) d
        float b = 2.0f * ray.direction.dot(oc);      // 2 * d (dot) (o - c)
        float c = oc.dot(oc) - radius * radius;      // (o - c) (dot) (o - c) - r^2

        float discriminant = b * b - 4 * a * c;
        if (discriminant < 0.0f){
            return false;
        }

        float sqrtD = std::sqrt(discriminant);

        float t1 = (-b - sqrtD) / (2 * a);
        float t2 = (-b + sqrtD) / (2 * a);

        float closest_t = std::numeric_limits<float>::infinity();

        if (t1 > EPS_HIT && t1 < closest_t) {
            closest_t = t1;
        }
        else if (t2 > EPS_HIT && t2 < closest_t) {
            closest_t = t2;
        }
        else {
            return false;
        }

        // Check if this intersection is closer than any previous hit
        if (closest_t < hit.t) {
            hit.hit = true;
            hit.t = closest_t;
            hit.point = ray.origin + ray.direction * closest_t;
            hit.normal = (hit.point - center) / radius;  // (p−c)/r
            hit.normal.normalize();
            return true;
        }

        return false;
    }

};