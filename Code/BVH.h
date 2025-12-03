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
        
        // Compute bounds for all shapes
        for (size_t i = start; i < end; ++i) {
            box.expand(shapes[i]->bounds());
        }

        // Select splitting axis based
        int axis = box.longestAxis();
        auto comparator = (axis == 0) ? box_x_compare
                        : (axis == 1) ? box_y_compare
                                      : box_z_compare;

        size_t object_span = end - start;

        // Base case
        if (object_span == 1) {
            left = shapes[start];
            right = nullptr;
        } 
        // Recursive case
        else {
            std::sort(shapes.begin() + start, shapes.begin() + end, comparator);
            
            size_t mid = start + object_span / 2;
        
            left = new BVHNode(shapes, start, mid);  
            right = new BVHNode(shapes, mid, end);   
        }
    }

    virtual ~BVHNode() {
        if (right != nullptr) {
        delete left;
        delete right;
        }
    }

    bool intersect(const Ray& ray, HitInfo& hit) const override {
        
        // Check box intersection
        if (!box.intersect(ray, hit.t)) {
            return false;
        }

        // If leaf, check actual shape
        if (right == nullptr) {
            return left->intersect(ray, hit);
        }
        
        // If internal node, check both children
        bool hit_left = left->intersect(ray, hit);
        bool hit_right = right->intersect(ray, hit);

        return hit_left || hit_right;
    }

    AABB bounds() const override { 
        return box; 
    }

    Vector3 centroid() const override { 
        return box.centre(); 
    }
};

#endif