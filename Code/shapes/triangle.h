#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "shape.h"
#include <cmath>
#include <algorithm>
#include <limits>

class Triangle : public Shape {
public:
    Vector3 v0, v1, v2; 
    Vector3 normal;

    Triangle(const Vector3& a, const Vector3& b, const Vector3& c)
        : v0(a), v1(b), v2(c)
    {
        // Compute normal
        normal = (v1 - v0).cross(v2 - v0);
        normal.normalize();
    }

    bool intersect(const Ray& ray, HitInfo& hit) const override {

        float denom = normal.dot(ray.direction);
        
        if (std::fabs(denom) < EPS_HIT) return false; // parallel

        // Solve for t: (P - V0) dot N = 0
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

        // Any point on a traingle can be determined by u, v, 1-u-v
        float invDenom = 1.0f / (dot11 * dot22 - dot12 * dot12);
        float u = (dot22 * dot1p - dot12 * dot2p) * invDenom;
        float v = (dot11 * dot2p - dot12 * dot1p) * invDenom;

        // u >= 0, v >= 0, and u + v <= 1
        if (u < 0.0f || v < 0.0f || (u + v) > 1.0f)
            return false;

        // Update HitInfo
        hit.hit = true;
        hit.t = t;
        hit.point = p;
        hit.normal = normal;
        
        // Flip normal if ray hits the backface
        if (denom > 0.0f) hit.normal = -hit.normal;

        hit.shape = (Shape*)this;

        hit.u = u;
        hit.v = v;

        return true;
    }

    Vector3 centroid() const override {
        // Average of three vertices
        return (v0 + v1 + v2) / 3.0f;
    }

    AABB bounds() const override {
        AABB box;
        box.expand(v0);
        box.expand(v1);
        box.expand(v2);
        return box;
    }
};

#endif