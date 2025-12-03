#include <fstream>
#include <iostream>
#include <string>

#include "scene.h"
#include "image.h"    
#include "shapes/sphere.h"
#include "shapes/cube.h"
#include "shapes/plane.h"

// Scene loader
bool loadScene(const std::string& filename, Camera& cam, Scene& scene)
{
    // Open file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open scene file " << filename << std::endl;
        return false;
    }

    std::string label;;

    while (file >> label) {

        // --- CAMERA ---
        if (label == "BEGIN_CAMERA") {

            // Handled by camera.cpp

            file.close();
            cam.readFromFile(filename);

            file.open(filename);
            std::string skip;
            while (file >> skip && skip != "END_CAMERA");
            continue;
        }

        // --- CUBE ---
        if (label == "BEGIN_CUBE") {

            std::string token;
            Vector3 translation(0,0,0);
            Vector3 rotation(0,0,0);
            Vector3 scale(1,1,1);

            Material mat;
            
            while (file >> token && token != "END_CUBE") {
                if (token == "translation") file >> translation.x >> translation.y >> translation.z;
                else if (token == "rotation") file >> rotation.x >> rotation.y >> rotation.z;
                else if (token == "scale") file >> scale.x >> scale.y >> scale.z;

                else if (token == "diffuse") file >> mat.diffuse.x >> mat.diffuse.y >> mat.diffuse.z;
                else if (token == "specular") file >> mat.specular.x >> mat.specular.y >> mat.specular.z;
                else if (token == "shininess") file >> mat.shininess;
                else if (token == "roughness") file >> mat.roughness; 
                else if (token == "reflectivity") file >> mat.reflectivity;
                else if (token == "transparency") file >> mat.transparency;
                else if (token == "ior") file >> mat.ior;

                else if (token == "texture") {
                    file >> mat.textureName;
                    if (!mat.textureName.empty() && mat.textureName != "none") {

                        std::string texturePath = "../Textures/" + mat.textureName;
                        mat.texture = new Image(texturePath);
                        
                        if (mat.texture->width == 0) {
                             std::cerr << "Failed to load texture: " << mat.textureName << ". Check file exists." << std::endl;
                             delete mat.texture;
                             mat.texture = nullptr;
                        }
                    }
                }
            }

            Cube* c = new Cube(translation, rotation, scale);
            c->material = mat; 
            scene.shapes.push_back(c);
            continue;
        }

        // --- SPHERE ---
        if (label == "BEGIN_SPHERE") {

            std::string token;
            Vector3 translation(0,0,0);
            Vector3 rotation(0,0,0);
            Vector3 scale(1,1,1);

            Material mat;


            while (file >> token && token != "END_SPHERE") {
                if (token == "translation") file >> translation.x >> translation.y >> translation.z;
                else if (token == "rotation") file >> rotation.x >> rotation.y >> rotation.z;
                else if (token == "scale") file >> scale.x >> scale.y >> scale.z;

                else if (token == "diffuse") file >> mat.diffuse.x >> mat.diffuse.y >> mat.diffuse.z;
                else if (token == "specular") file >> mat.specular.x >> mat.specular.y >> mat.specular.z;
                else if (token == "shininess") file >> mat.shininess;
                else if (token == "roughness") file >> mat.roughness; 
                else if (token == "reflectivity") file >> mat.reflectivity;
                else if (token == "transparency") file >> mat.transparency;
                else if (token == "ior") file >> mat.ior;

                else if (token == "texture") {
                    file >> mat.textureName;
                    if (!mat.textureName.empty() && mat.textureName != "none") {

                        std::string texturePath = "../Textures/" + mat.textureName;
                        mat.texture = new Image(texturePath);
                        if (mat.texture->width == 0) {
                             std::cerr << "Failed to load texture: " << mat.textureName << ". Check file exists." << std::endl;
                             delete mat.texture;
                             mat.texture = nullptr;
                        }
                    }
                }
            }

            
            Sphere* s = new Sphere(translation, rotation, scale);
            s->material = mat;
            scene.shapes.push_back(s);
            continue;
        }

        if (label == "BEGIN_PLANE") {
            std::vector<Vector3> verts;
            std::string token;
            
            Material mat;

            while (file >> token && token != "END_PLANE") {
                if (token == "vertex") {
                    float x, y, z;
                    file >> x >> y >> z;
                    verts.emplace_back(x, y, z);
                }

                else if (token == "diffuse") file >> mat.diffuse.x >> mat.diffuse.y >> mat.diffuse.z;
                else if (token == "specular") file >> mat.specular.x >> mat.specular.y >> mat.specular.z;
                else if (token == "shininess") file >> mat.shininess;
                else if (token == "roughness") file >> mat.roughness; 
                else if (token == "reflectivity") file >> mat.reflectivity;
                else if (token == "transparency") file >> mat.transparency;
                else if (token == "ior") file >> mat.ior;

                else if (token == "texture") {
                    file >> mat.textureName;
                    if (!mat.textureName.empty() && mat.textureName != "none") {

                        std::string texturePath = "../Textures/" + mat.textureName;
                        mat.texture = new Image(texturePath);
                        if (mat.texture->width == 0) {
                             std::cerr << "Failed to load texture: " << mat.textureName << ". Check file exists." << std::endl;
                             delete mat.texture;
                             mat.texture = nullptr;
                        }
                    }
                }
            }

            if (verts.size() == 4) {
                Plane* p = new Plane(verts[0], verts[1], verts[2], verts[3]);
                p->material = mat;
                scene.shapes.push_back(p);
            }
            continue;

            continue;
        }

        // --- LIGHT ---
        if (label == "BEGIN_LIGHT") {
            std::string token;
            Vector3 location(0,0,0);
            float intensity = 1.0f;
            float radius = 0.0f;

            while (file >> token && token != "END_LIGHT") {
                if (token == "location")
                    file >> location.x >> location.y >> location.z;
                else if (token == "intensity")
                    file >> intensity;
                else if (token == "radius")
                    file >> radius;
            }

            Light light;
            light.position = location;
            float scaleFactor = 10.0f; 
            light.intensity = Vector3(intensity, intensity, intensity) / scaleFactor;
            light.radius = radius; 
            scene.lights.push_back(light);
            continue;
        }
    }

    file.close();

    return true;
}
