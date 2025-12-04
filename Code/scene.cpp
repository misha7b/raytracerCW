#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include "scene.h"
#include "image.h"    
#include "shapes/sphere.h"
#include "shapes/cube.h"
#include "shapes/plane.h"
#include "shapes/triangle.h"

// Mesh loader (OBJ)
void loadMesh(const std::string& filepath, const Vector3& translation, float scale, const Material& mat, Scene& scene) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open mesh file: " << filepath << "\n";
        return;
    }

    std::vector<Vector3> vertices;
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        // Vertex:
        if (type == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            
            // Apply scale and translation 
            Vector3 v(x, y, z);
            vertices.push_back(translation + v * scale); 
        }
        // Face 
        else if (type == "f") {
            std::string v1_str, v2_str, v3_str;
            iss >> v1_str >> v2_str >> v3_str;
            
            // Convert to 0-based index
            int idx1 = std::stoi(v1_str) - 1;
            int idx2 = std::stoi(v2_str) - 1;
            int idx3 = std::stoi(v3_str) - 1;

            if (idx1 >= 0 && idx3 < vertices.size()) {
                // Create triangle and add to scene
                Triangle* tri = new Triangle(vertices[idx1], vertices[idx2], vertices[idx3]);
                tri->material = mat;
                scene.shapes.push_back(tri);
            }
        }
    }
    std::cout << "Loaded mesh: " << filepath << " (" << vertices.size() << " vertices)\n";
}



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
        }

        // --- MESH (OBJ) ---
        if (label == "BEGIN_MESH") {
            std::string token;
            std::string objFilename;
            Vector3 translation(0,0,0);
            float scale = 1.0f;
            Material mat;

            while (file >> token && token != "END_MESH") {
                if (token == "file") {
                    file >> objFilename;
                    if (objFilename.find('/') == std::string::npos && objFilename.find('\\') == std::string::npos) {
                        objFilename = "../Meshes/" + objFilename;
                    }
                }
                else if (token == "translation") file >> translation.x >> translation.y >> translation.z;
                else if (token == "scale") file >> scale;
                
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
                             delete mat.texture;
                             mat.texture = nullptr;
                        }
                    }
                }
            }

            if (!objFilename.empty()) {
                loadMesh(objFilename, translation, scale, mat, scene);
            }
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

    std::cout << "Scene loaded successfully" << std::endl;
    file.close();

    return true;
}
