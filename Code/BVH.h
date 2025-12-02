#ifndef BVH_H
#define BVH_H

#include "shapes/shape.h"
#include <vector>      
#include <algorithm>   
#include <iostream>    

inline bool box_x_compare(const Shape* a, const Shape* b) {
    return a->centroid().x < b->centroid().x;
}
inline bool box_y_compare(const Shape* a, const Shape* b) {
    return a->centroid().y < b->centroid().y;
}
inline bool box_z_compare(const Shape* a, const Shape* b) {
    return a->centroid().z < b->centroid().z;
}

class BVHNode : public Shape {
public:
    Shape* left;
    Shape* right;
    AABB box;

    BVHNode(std::vector<Shape*>& shapes, size_t start, size_t end) {
        
        for (size_t i = start; i < end; ++i) {
            box.expand(shapes[i]->bounds());
        }

        int axis = box.longestAxis();
        auto comparator = (axis == 0) ? box_x_compare
                        : (axis == 1) ? box_y_compare
                                      : box_z_compare;

        size_t object_span = end - start;

        if (object_span == 1) {
            left = shapes[start];
            right = nullptr;
        } 
        else {
            std::sort(shapes.begin() + start, shapes.begin() + end, comparator);
            
            size_t mid = start + object_span / 2;
        
            left = new BVHNode(shapes, start, mid);  
            right = new BVHNode(shapes, mid, end);   
        }
    }

    virtual ~BVHNode() {
        delete left;
        delete right;
    }

    bool intersect(const Ray& ray, HitInfo& hit) const override {
        
        if (!box.intersect(ray, hit.t)) {
            return false;
        }

        if (right == nullptr) {
            return left->intersect(ray, hit);
        }
        
        bool hit_left = left->intersect(ray, hit);
        bool hit_right = right->intersect(ray, hit);

        return hit_left || hit_right;
    }

    AABB bounds() const override { 
        return box; 
    }

    Vector3 centroid() const override { 
        return box.center(); 
    }
};

#endif