#include "shape.h"
#include <algorithm>
#include <cmath>
#include <limits>

class Cube : public Shape {
public:
    Vector3 translation;  
    Matrix3 rotation;    
    float radius;
    
    Cube(const Vector3& t, const Vector3& eulerRadians, float r)
    : translation(t), radius(r)
{
    rotation = Matrix3::fromEuler(
        eulerRadians.x,
        eulerRadians.y,
        eulerRadians.z
    );
}

    bool intersect(const Ray& ray, HitInfo& hit) const override {

        Matrix3 reverse_rotation = rotation.transpose();
        Vector3 o_local = reverse_rotation * (ray.origin - translation);
        Vector3 d_local = reverse_rotation * ray.direction;

        float t_min = -std::numeric_limits<float>::infinity();
        float t_max =  std::numeric_limits<float>::infinity();


        if (std::fabs(d_local.x) < EPS_DIR) {
            // Ray is parallel to the x face
            if (o_local.x < -radius || o_local.x > radius) return false;
        } else {
            float t0x = (-radius - o_local.x) / d_local.x;
            float t1x = ( radius - o_local.x) / d_local.x;
            if (t0x > t1x) std::swap(t0x, t1x);
            t_min = std::max(t_min, t0x);
            t_max = std::min(t_max, t1x);
            if (t_max < t_min) return false;
        }

        if (std::fabs(d_local.y) < EPS_DIR) {
            // Ray is parallel to the y face
            if (o_local.y < -radius || o_local.y > radius) return false;
        } else {
            float t0y = (-radius - o_local.y) / d_local.y;
            float t1y = ( radius - o_local.y) / d_local.y;
            if (t0y > t1y) std::swap(t0y, t1y);
            t_min = std::max(t_min, t0y);
            t_max = std::min(t_max, t1y);
            if (t_max < t_min) return false;
        }

        if (std::fabs(d_local.z) < EPS_DIR) {
            // Ray is parallel to the z face
            if (o_local.z < -radius || o_local.z > radius) return false;
        } else {
            float t0z = (-radius - o_local.z) / d_local.z;
            float t1z = ( radius - o_local.z) / d_local.z;
            if (t0z > t1z) std::swap(t0z, t1z);
            t_min = std::max(t_min, t0z);
            t_max = std::min(t_max, t1z);
            if (t_max < t_min) return false;
        }

        float t_hit;
        if (t_min > EPS_HIT)      t_hit = t_min; 
        else if (t_max > EPS_HIT) t_hit = t_max;  
        else                      return false;  

        if (t_hit >= hit.t) return false;   

        Vector3 p_local = o_local + d_local * t_hit;

        Vector3 n_local(0, 0, 0);
        float dx = std::fabs(std::fabs(p_local.x) - radius);
        float dy = std::fabs(std::fabs(p_local.y) - radius);
        float dz = std::fabs(std::fabs(p_local.z) - radius);

        if (dx <= dy && dx <= dz)
            n_local.x = (p_local.x > 0.0f) ? 1.0f : -1.0f;
        else if (dy <= dx && dy <= dz)
            n_local.y = (p_local.y > 0.0f) ? 1.0f : -1.0f;
        else
            n_local.z = (p_local.z > 0.0f) ? 1.0f : -1.0f;

        Vector3 p_world = translation + rotation * p_local;
        Vector3 n_world = rotation * n_local;
        n_world.normalize();

        hit.hit    = true;
        hit.t      = t_hit;
        hit.point  = p_world;
        hit.normal = n_world;

        return true;

    }

};