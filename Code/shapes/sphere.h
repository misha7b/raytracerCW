#ifndef SPHERE_H
#define SPHERE_H

#include "shape.h"
#include <cmath>
#include <algorithm>

class Sphere : public Shape {
public:
    Vector3 translation;
    Matrix3 rotation;
    Vector3 scale;
    
    // Calculate rotation matrix
    Sphere(const Vector3& t, const Vector3& eulerRadians, const Vector3& s)
        : translation(t), scale(s)
    {
        rotation = Matrix3::fromEuler(eulerRadians.x,
                                      eulerRadians.y,
                                      eulerRadians.z);
    }

    bool intersect(const Ray& ray, HitInfo& hit) const override {

        // Transform ray to local space 
        Matrix3 reverse_rotation = rotation.transpose(); // Inverse of orthogonal rotation matrix
        Vector3 o_local = reverse_rotation * (ray.origin - translation);
        Vector3 d_local = reverse_rotation * ray.direction;

        // Scale
        o_local = o_local / scale;
        d_local = d_local / scale;

        // Solve |O + tD|^2 = 1
        float a = d_local.dot(d_local);         // d (dot) d
        float b = 2.0f * o_local.dot(d_local);  // 2 * d (dot) (o - c)
        float c = o_local.dot(o_local) - 1.0f;  // (o - c) (dot) (o - c) - r^2

        float discriminant = b * b - 4 * a * c;
        if (discriminant < 0.0f){
            return false;
        }

        float sqrtD = std::sqrt(discriminant);

        // 2 intersection points (roots)
        float t1 = (-b - sqrtD) / (2 * a);
        float t2 = (-b + sqrtD) / (2 * a);

        float closest_t = std::numeric_limits<float>::infinity();

        // Choose closest intersectiob
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

            Vector3 p_local = o_local + d_local * closest_t;
            Vector3 n_local = p_local;
            n_local.normalize();

            // Return to world space
            Vector3 p_world = translation + rotation * (p_local * scale);

            Vector3 n_world = rotation * (n_local / scale);
            n_world.normalize();

            // Update HitInfo
            hit.hit = true;
            hit.t = closest_t;
            hit.point = p_world;
            hit.normal = n_world;
            hit.shape = (Shape*)this;

            // Convert point on sphere to coordinates
            float theta = std::atan2(n_local.x, n_local.z);
            float phi   = std::acos(n_local.y);

            hit.u = (theta + M_PI) / (2.0f * M_PI);
            hit.v = phi / M_PI;

            return true;
        }

        return false;
    }

    Vector3 centroid() const override {
        return translation;
    }
    
    // Calculate AABB box
    AABB bounds() const override {

        AABB box;
        Vector3 v[8];
        v[0] = Vector3(-scale.x, -scale.y, -scale.z);
        v[1] = Vector3( scale.x, -scale.y, -scale.z);
        v[2] = Vector3( scale.x,  scale.y, -scale.z);
        v[3] = Vector3(-scale.x,  scale.y, -scale.z);
        v[4] = Vector3(-scale.x, -scale.y,  scale.z);
        v[5] = Vector3( scale.x, -scale.y,  scale.z);
        v[6] = Vector3( scale.x,  scale.y,  scale.z);
        v[7] = Vector3(-scale.x,  scale.y,  scale.z);

        for (int i = 0; i < 8; ++i) {
            box.expand(translation + rotation * v[i]);
        }
        return box;
    }


};

#endif
