#include <iostream>
#include <string>
#include <vector>
#include <cstring> 
#include <cmath>  
#include <chrono>

#include "raytracer.h"
#include "scene.h"
#include "image.h"
#include "BVH.h"
#include "config.h" 


RenderConfig parseArguments(int argc, char* argv[]) {
    RenderConfig config;

    config.inputScene = "../ASCII/Test1.txt";
    config.outputImage = "../Output/output.ppm";
    config.samplesPerPixel = 1;           
    config.maxDepth = 3;
    config.useBVH = true;
    config.width = 0;                  
    config.height = 0;
    config.shadowSamples = 1;     

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            config.inputScene = argv[++i];
        } 
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            std::string filename = argv[++i];
            
            if (filename.find('/') == std::string::npos && filename.find('\\') == std::string::npos) {
                config.outputImage = "../Output/" + filename;
            } else {
                config.outputImage = filename;
            }
        }
        else if (strcmp(argv[i], "-w") == 0 && i + 1 < argc) {
            config.width = std::stoi(argv[++i]);
        } 
        else if (strcmp(argv[i], "-h") == 0 && i + 1 < argc) {
            config.height = std::stoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-spp") == 0 && i + 1 < argc) {
            config.samplesPerPixel = std::stoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            config.maxDepth = std::stoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-no-bvh") == 0) {
            config.useBVH = false;
        }
        else if (strcmp(argv[i], "--shadow-samples") == 0 && i + 1 < argc) {
    config.shadowSamples = std::stoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--glossy-samples") == 0 && i + 1 < argc) {
            config.glossySamples = std::stoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--glossy-roughness") == 0 && i + 1 < argc) {
            config.glossyRoughness = std::stof(argv[++i]);
        }
    }
    return config;
}


int main(int argc, char* argv[]) {

    // set random seed
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // read config
    RenderConfig config = parseArguments(argc, argv);

    std::cout << "--- Raytracer System ---\n";
    std::cout << "Scene:   " << config.inputScene << "\n";
    std::cout << "Output:  " << config.outputImage << "\n";
    std::cout << "BVH:     " << (config.useBVH ? "On" : "Off") << "\n";
    std::cout << "Samples: " << config.samplesPerPixel << " spp\n";

    Camera cam;
    Scene scene;

    if (!loadScene(config.inputScene, cam, scene)) {
        std::cerr << "Error: Scene failed to load: " << config.inputScene << "\n";
        return 1;
    }


    if (config.width > 0) cam.resolutionX = config.width;
    if (config.height > 0) cam.resolutionY = config.height;


    BVHNode* bvh_root = nullptr;
    if (config.useBVH) {
        std::cout << "Shapes in scene: " << scene.shapes.size() << "\n";
        std::cout << "Building BVH...\n";

    bvh_root = new BVHNode(scene.shapes, 0, scene.shapes.size());
}

    Raytracer tracer(&cam, &scene, bvh_root, config);

    Image img(cam.resolutionX, cam.resolutionY);
    
    std::cout << "Rendering at " << cam.resolutionX << "x" << cam.resolutionY << "...\n";
    auto start_time = std::chrono::high_resolution_clock::now();

    tracer.render(img); 

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    
    std::cout << "Render Time: " << elapsed.count() << " seconds\n";

    if (img.writePPM(config.outputImage)) {
        std::cout << "Saved to " << config.outputImage << "\n";
    } else {
        std::cerr << "Failed to save image!\n";
    }

    if (bvh_root) delete bvh_root;
    
    return 0;
}