#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "shapes/shape.h"
#include <vector>
#include <string>

bool loadScene(const std::string& filename,
               Camera& cam,
               std::vector<Shape*>& shapes);

#endif
