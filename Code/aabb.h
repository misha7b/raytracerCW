#ifndef AABB_H
#define AABB_H

#include "maths.h"
#include <algorithm>
#include <limits>
#include <cmath>

struct AABB {
    Vector3 min;
    Vector3 max;

    AABB() : min(+INFINITY, +INFINITY, +INFINITY),
             max(-INFINITY, -INFINITY, -INFINITY) {}

    AABB(const Vector3& mi, const Vector3& ma) : min(mi), max(ma) {}

    // Expand the AABB to include a single point
    void expand(const Vector3& p) {
        min.x = std::min(min.x, p.x);
        min.y = std::min(min.y, p.y);
        min.z = std::min(min.z, p.z);
        max.x = std::max(max.x, p.x);
        max.y = std::max(max.y, p.y);
        max.z = std::max(max.z, p.z);
    }

    // Expand the AABB to include another AABB
    void expand(const AABB& b) {
        expand(b.min);
        expand(b.max);
    }

    // Expand the AABB to include a single point
    static AABB combine(const AABB& a, const AABB& b) {
        AABB result = a;
        result.expand(b);
        return result;
    }

    // Compute centre
    Vector3 centre() const { return (min + max) * 0.5f; }

    // Compute extent
    Vector3 extent() const { return (max - min) * 0.5f; }

    // Return longest axis
    int longestAxis() const {
        Vector3 e = max - min;
        if (e.x >= e.y && e.x >= e.z) return 0;
        if (e.y >= e.x && e.y >= e.z) return 1;
        return 2;
    }

    // Ray-AABB slab test
    bool intersect(const Ray& r, float tMax) const {
        float tmin = 0.0f; // enter distance
        float tmax = tMax; // exit distance

        for (int i = 0; i < 3; ++i) {
            float o, d, mi, ma;
            if (i == 0) { o = r.origin.x; d = r.direction.x; mi = min.x; ma = max.x; }
            else if (i == 1) { o = r.origin.y; d = r.direction.y; mi = min.y; ma = max.y; }
            else { o = r.origin.z; d = r.direction.z; mi = min.z; ma = max.z; }

            // Handle rays nearly parallel to slab to avoid inf/NaN
            if (std::abs(d) < 1e-8f) {
                // If origin is outside the slab, no hit
                if (o < mi || o > ma) return false;
                // Otherwise, this axis does not clip the interval
                continue;
            }

            float invD = 1.0f / d;
            float t0 = (mi - o) * invD;
            float t1 = (ma - o) * invD;
            
            if (invD < 0.0f) std::swap(t0, t1); // Swap if direction is negative

            tmin = std::max(tmin, t0);
            tmax = std::min(tmax, t1);
            
            // Check for valid interval
            if (tmax < tmin) return false;
        }
        return true;
    }
};

#endif
