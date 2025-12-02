#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "shapes/shape.h"
#include <vector>
#include <string>

struct Light {
    Vector3 position;
    Vector3 intensity;
    float radius = 0.0f;
};

struct Scene {
    std::vector<Shape*> shapes;
    std::vector<Light> lights;
};

bool loadScene(const std::string& filename, Camera& cam, Scene& scene);

#endif
