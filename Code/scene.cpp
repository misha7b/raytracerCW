#include "scene.h"
#include "shapes/sphere.cpp"
#include "shapes/cube.cpp"
#include "shapes/plane.cpp"
#include <fstream>
#include <iostream>
#include <string>

bool loadScene(const std::string& filename, Camera& cam, std::vector<Shape*>& shapes)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open scene file " << filename << std::endl;
        return false;
    }

    std::string label;

    Vector3 translation, rotation, location;
    float scale = 1.0f;
    float radius = 1.0f;

    while (file >> label) {

        // --- CAMERA ---
        if (label == "BEGIN_CAMERA") {
            // Camera::readFromFile
            file.close();
            cam.readFromFile(filename);

            file.open(filename);
            std::string skip;
            while (file >> skip && skip != "END_CAMERA");
            continue;
        }

        // --- CUBE ---
        if (label == "BEGIN_CUBE") {
            translation = Vector3(0, 0, 0);
            rotation    = Vector3(0, 0, 0);
            scale       = 1.0f;

            while (file >> label && label != "END_CUBE") {
                if (label == "translation")
                    file >> translation.x >> translation.y >> translation.z;
                else if (label == "rotation")
                    file >> rotation.x >> rotation.y >> rotation.z;
                else if (label == "scale")
                    file >> scale;
            }

            shapes.push_back(new Cube(translation, rotation, scale));
            continue;
        }

        // --- SPHERE ---
        if (label == "BEGIN_SPHERE") {
            location = Vector3(0, 0, 0);
            radius   = 1.0f;

            while (file >> label && label != "END_SPHERE") {
                if (label == "location")
                    file >> location.x >> location.y >> location.z;
                else if (label == "radius")
                    file >> radius;
            }

            shapes.push_back(new Sphere(location, radius));
            continue;
        }

        if (label == "BEGIN_PLANE") {
            std::vector<Vector3> verts;
            std::string token;

            while (file >> token && token != "END_PLANE") {
                if (token == "vertex") {
                    float x, y, z;
                    file >> x >> y >> z;
                    verts.emplace_back(x, y, z);
                }
            }

            if (verts.size() == 4) {
                shapes.push_back(new Plane(verts[0], verts[1], verts[2], verts[3]));
            }

            continue;
        }
    }

    file.close();

    return true;
}
