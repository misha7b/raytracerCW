#include "shape.h"
#include <cmath>
#include <algorithm>
#include <limits>

class Plane : public Shape {
public:
    Vector3 v0, v1, v2, v3; 
    Vector3 normal;

    Plane(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d)
        : v0(a), v1(b), v2(c), v3(d)
    {

        normal = (v1 - v0).cross(v2 - v0);
        normal.normalize();
    }

    bool intersect(const Ray& ray, HitInfo& hit) const override {

        float denom = normal.dot(ray.direction);
        if (std::fabs(denom) < EPS_HIT) return false; // parallel

        float t = normal.dot(v0 - ray.origin) / denom;
        if (t < EPS_HIT || t >= hit.t) return false;

        Vector3 p = ray.origin + ray.direction * t;

        Vector3 edge1 = v1 - v0;
        Vector3 edge2 = v2 - v0;
        Vector3 vp = p - v0;

        float dot11 = edge1.dot(edge1);
        float dot22 = edge2.dot(edge2);
        float dot12 = edge1.dot(edge2);
        float dot1p = edge1.dot(vp);
        float dot2p = edge2.dot(vp);

        float invDenom = 1.0f / (dot11 * dot22 - dot12 * dot12);
        float u = (dot22 * dot1p - dot12 * dot2p) * invDenom;
        float v = (dot11 * dot2p - dot12 * dot1p) * invDenom;

        if (u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f)
            return false;

        hit.hit = true;
        hit.t = t;
        hit.point = p;
        hit.normal = normal;

        return true;
    }
};
