#include "shape.h"
#include <algorithm>
#include <cmath>
#include <limits>

class Cube : public Shape {
public:
    Vector3 translation;
    Matrix3 rotation;
    Vector3 halfExtent;
    
    Cube(const Vector3& t, const Vector3& eulerRadians, const Vector3& scale)
        : translation(t), halfExtent(scale)
    {
        rotation = Matrix3::fromEuler(eulerRadians.x,
                                      eulerRadians.y,
                                      eulerRadians.z);
    }


    bool intersect(const Ray& ray, HitInfo& hit) const override {

        Matrix3 reverse_rotation = rotation.transpose();
        Vector3 o_local = reverse_rotation * (ray.origin - translation);
        Vector3 d_local = reverse_rotation * ray.direction;

        float t_min = -std::numeric_limits<float>::infinity();
        float t_max =  std::numeric_limits<float>::infinity();


        // X slab
        if (std::fabs(d_local.x) < EPS_DIR) {
            if (o_local.x < -halfExtent.x || o_local.x > halfExtent.x) return false;
        } else {
            float t0 = (-halfExtent.x - o_local.x) / d_local.x;
            float t1 = ( halfExtent.x - o_local.x) / d_local.x;
            if (t0 > t1) std::swap(t0, t1);
            t_min = std::max(t_min, t0);
            t_max = std::min(t_max, t1);
            if (t_max < t_min) return false;
        }

        // Y slab
        if (std::fabs(d_local.y) < EPS_DIR) {
            if (o_local.y < -halfExtent.y || o_local.y > halfExtent.y) return false;
        } else {
            float t0 = (-halfExtent.y - o_local.y) / d_local.y;
            float t1 = ( halfExtent.y - o_local.y) / d_local.y;
            if (t0 > t1) std::swap(t0, t1);
            t_min = std::max(t_min, t0);
            t_max = std::min(t_max, t1);
            if (t_max < t_min) return false;
        }

        // Z slab
        if (std::fabs(d_local.z) < EPS_DIR) {
            if (o_local.z < -halfExtent.z || o_local.z > halfExtent.z) return false;
        } else {
            float t0 = (-halfExtent.z - o_local.z) / d_local.z;
            float t1 = ( halfExtent.z - o_local.z) / d_local.z;
            if (t0 > t1) std::swap(t0, t1);
            t_min = std::max(t_min, t0);
            t_max = std::min(t_max, t1);
            if (t_max < t_min) return false;
        }

        float t_hit;
        if (t_min > EPS_HIT)      t_hit = t_min; 
        else if (t_max > EPS_HIT) t_hit = t_max;  
        else                      return false;  

        if (t_hit >= hit.t) return false;   

        Vector3 p_local = o_local + d_local * t_hit;

        Vector3 n_local(0, 0, 0);
        float dx = std::fabs(std::fabs(p_local.x) - halfExtent.x);
        float dy = std::fabs(std::fabs(p_local.y) - halfExtent.y);
        float dz = std::fabs(std::fabs(p_local.z) - halfExtent.z);

        float u = 0.0f;
        float v = 0.0f;

        if (dx <= dy && dx <= dz) {
            // Hit X face
            n_local.x = (p_local.x > 0.0f) ? 1.0f : -1.0f;
            // Map Z and Y to U and V
            u = 0.5f * (p_local.z / halfExtent.z + 1.0f);
            v = 0.5f * (p_local.y / halfExtent.y + 1.0f);
        }
        else if (dy <= dx && dy <= dz) {
            // Hit Y face
            n_local.y = (p_local.y > 0.0f) ? 1.0f : -1.0f;
            // Map X and Z to U and V
            u = 0.5f * (p_local.x / halfExtent.x + 1.0f);
            v = 0.5f * (p_local.z / halfExtent.z + 1.0f);
        }
        else {
            // Hit Z face
            n_local.z = (p_local.z > 0.0f) ? 1.0f : -1.0f;
            // Map X and Y to U and V
            u = 0.5f * (p_local.x / halfExtent.x + 1.0f);
            v = 0.5f * (p_local.y / halfExtent.y + 1.0f);
        }

        Vector3 p_world = translation + rotation * p_local;
        Vector3 n_world = rotation * n_local;
        n_world.normalize();

        hit.hit    = true;
        hit.t      = t_hit;
        hit.point  = p_world;
        hit.normal = n_world;
        hit.shape  = (Shape*)this;

        hit.u = u;
        hit.v = v;

        return true;

    }

    Vector3 centroid() const override {
        return translation;
    }

    AABB bounds() const override {
        AABB box;
    
        Vector3 v[8];
        v[0] = Vector3(-halfExtent.x, -halfExtent.y, -halfExtent.z);
        v[1] = Vector3( halfExtent.x, -halfExtent.y, -halfExtent.z);
        v[2] = Vector3( halfExtent.x,  halfExtent.y, -halfExtent.z);
        v[3] = Vector3(-halfExtent.x,  halfExtent.y, -halfExtent.z);
        v[4] = Vector3(-halfExtent.x, -halfExtent.y,  halfExtent.z);
        v[5] = Vector3( halfExtent.x, -halfExtent.y,  halfExtent.z);
        v[6] = Vector3( halfExtent.x,  halfExtent.y,  halfExtent.z);
        v[7] = Vector3(-halfExtent.x,  halfExtent.y,  halfExtent.z);

        for (int i = 0; i < 8; ++i) {
            box.expand(translation + rotation * v[i]);
        }
        return box;
    }

};