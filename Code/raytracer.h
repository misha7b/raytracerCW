#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "scene.h"
#include "BVH.h"
#include "image.h"
#include "config.h"

class Raytracer {
public:
    Raytracer(const Camera* cam, const Scene* scn, const BVHNode* bvh, const RenderConfig& cfg)
        : camera(cam), scene(scn), bvhRoot(bvh), config(cfg) {}

    // Main recursive function
    Vector3 traceRay(const Ray& ray, int depth) const;

    // Shading
    Vector3 shade(const Ray& ray, const HitInfo& hit, int depth) const;
    
    void render(Image& img) const;

private:
    const Camera* camera;
    const Scene* scene;
    const BVHNode* bvhRoot;
    RenderConfig config;
};

#endif