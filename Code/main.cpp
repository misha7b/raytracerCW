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

void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " [options]\n"
              << "Options:\n"
              << "  -i <file>        Input scene file (default: ../ASCII/Test1.txt)\n"
              << "  -o <file>        Output PPM file (default: ../Output/output.ppm)\n"
              << "  -w <int>         Output width (overrides scene)\n"
              << "  -h <int>         Output height (overrides scene)\n"
              << "  -spp <int>       Samples per pixel (default: 1)\n"
              << "  -d <int>         Max recursion depth (default: 3)\n"
              << "  -no-bvh          Disable BVH acceleration\n"
              << "  -no-shading      Disable lighting calculations (flat color only)\n"
              << "  --shadow-samples <int> Number of shadow rays for distributed RT\n"
              << "  --glossy-samples <int> Number of reflection rays for glossy materials\n"
              << "  -exposure <val>  Exposure multiplier (default: 1.0)\n";
}


RenderConfig parseArguments(int argc, char* argv[]) {
    RenderConfig config;

    // Defaults
    config.inputScene = "../ASCII/Test1.txt";
    config.outputImage = "../Output/output.ppm";
    config.samplesPerPixel = 1;           
    config.maxDepth = 3;
    config.useBVH = true;
    config.width = 0;   // use scene file defaults         
    config.height = 0;  // use scene file defaults 
    config.shadowSamples = 1;  
    config.exposure = 1.0f;   

    for (int i = 1; i < argc; ++i) {

        if (strcmp(argv[i], "--help") == 0) {
            printUsage(argv[0]);
            exit(0);
        }
        else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            std::string filename = argv[++i];
            if (filename.find('/') == std::string::npos && filename.find('\\') == std::string::npos) {
                config.inputScene = "../ASCII/" + filename;
            } else {
                config.inputScene = filename;
            }
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
        else if (strcmp(argv[i], "-no-shading") == 0) {
            config.noShading = true;
        }
        else if (strcmp(argv[i], "--shadow-samples") == 0 && i + 1 < argc) {
    config.shadowSamples = std::stoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--glossy-samples") == 0 && i + 1 < argc) {
            config.glossySamples = std::stoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-exposure") == 0 && i + 1 < argc) {
            config.exposure = std::stof(argv[++i]);
        }
    }
    return config;
}


int main(int argc, char* argv[]) {

    // Set random seed
    srand(static_cast<unsigned int>(time(nullptr)));
    
    RenderConfig config = parseArguments(argc, argv);

    std::cout << "========================================\n";
    std::cout << "Scene:      " << config.inputScene << "\n";
    std::cout << "Output:     " << config.outputImage << "\n";
    std::cout << "BVH:        " << (config.useBVH ? "Enabled" : "Disabled") << "\n";
    std::cout << "Depth:      " << config.maxDepth << "\n";
    std::cout << "AA Samples: " << config.samplesPerPixel << "\n";
    std::cout << "Exposure:   " << config.exposure << "\n";
    std::cout << "========================================\n";

    Camera cam;
    Scene scene;

    // Load scene
    if (!loadScene(config.inputScene, cam, scene)) {
        std::cerr << "Error: Scene failed to load: " << config.inputScene << "\n";
        return 1;
    }

    // Set camera resolution
    if (config.width > 0) cam.resolutionX = config.width;
    if (config.height > 0) cam.resolutionY = config.height;


    // Build BVH
    BVHNode* bvh_root = nullptr;
    if (config.useBVH) {
        std::cout << "Building BVH for " << scene.shapes.size() << " primitives...\n";
       
        bvh_root = new BVHNode(scene.shapes, 0, scene.shapes.size());
    }

    // Initialise renderer
    Raytracer tracer(&cam, &scene, bvh_root, config);
    Image img(cam.resolutionX, cam.resolutionY);
    
    // Render Loop
    std::cout << "Rendering started at " << cam.resolutionX << "x" << cam.resolutionY << "...\n";
    auto start_time = std::chrono::high_resolution_clock::now();

    tracer.render(img); 

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    
    std::cout << "Render Complete\n";
    std::cout << "Time Taken: " << elapsed.count() << " seconds\n";

    if (img.writePPM(config.outputImage)) {
        std::cout << "Saved to " << config.outputImage << "\n";
    } else {
        std::cerr << "Failed to save image!\n";
    }

    if (bvh_root) delete bvh_root;
    
    return 0;
}