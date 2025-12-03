#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct RenderConfig {
    // Default settings
    int width = 0;              // use camera default
    int height = 0;
    int maxDepth = 3;
    int samplesPerPixel = 1;    // 1 = no AA
    bool useBVH = true;
    bool useShadows = true;    

    int shadowSamples = 1;      
    int glossySamples = 1;     
    
    std::string inputScene = "Test1.txt";
    std::string outputImage = "output.ppm";
};

#endif