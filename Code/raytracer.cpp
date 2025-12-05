
#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>

#include "raytracer.h"
#include "maths.h"

const float SHADOW_BIAS = 0.001;
const float REFLECTION_BIAS = 0.001;
const Vector3 BACKGROUND_COLOR(0.3f, 0.3f, 0.3f);

// Sample a random point on light source
Vector3 samplePointOnLight(const Light& light, const Vector3& target, int gridX, int gridY, int gridSize) {
    if (light.radius <= 0.0f) return light.position;

    // Calculate direction from light to targer
    Vector3 forward = target - light.position;
    forward.normalize();

    Vector3 globalUp = Vector3(0, 1, 0);
    
    if (std::abs(forward.y) > 0.99f) {
        globalUp = Vector3(1, 0, 0);
    }

    Vector3 right = globalUp.cross(forward);
    right.normalize();

    Vector3 up = forward.cross(right);
    up.normalize();

    // Stratified sampling
    float cellSize = 1.0f / static_cast<float>(gridSize);
    float r1 = (gridX * cellSize) + (randomFloat() * cellSize);
    float r2 = (gridY * cellSize) + (randomFloat() * cellSize);

    float r = light.radius * std::sqrt(r1); 
    float theta = 2.0f * M_PI * r2;

    float x = r * cos(theta);
    float y = r * sin(theta);

    return light.position + (right * x) + (up * y);
}

// Sample a random direction
Vector3 sampleUnitSphere(int gridX, int gridY, int gridSize) {
    float cellSize = 1.0f / static_cast<float>(gridSize);

    float r1 = (gridX * cellSize) + (randomFloat() * cellSize);
    float r2 = (gridY * cellSize) + (randomFloat() * cellSize);


    float theta = 2.0f * M_PI * r1;
    
    float z = 1.0f - 2.0f * r2;
    
    float r = std::sqrt(1.0f - z * z);

    return Vector3(r * cos(theta), r * sin(theta), z);
}

// Reinhard
// C = C / (1 + C)
Vector3 reinhardToneMapping(const Vector3& x) {
    return Vector3(
        x.x / (1.0f + x.x),
        x.y / (1.0f + x.y),
        x.z / (1.0f + x.z)
    );
}


// ACES 
// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
Vector3 acesToneMapping(const Vector3& x) {
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;

    return Vector3(
        std::clamp((x.x * (a * x.x + b)) / (x.x * (c * x.x + d) + e), 0.0f, 1.0f),
        std::clamp((x.y * (a * x.y + b)) / (x.y * (c * x.y + d) + e), 0.0f, 1.0f),
        std::clamp((x.z * (a * x.z + b)) / (x.z * (c * x.z + d) + e), 0.0f, 1.0f)
    );
}

Vector3 Raytracer::traceRay(const Ray& ray, int depth) const {
    HitInfo hit;
    hit.hit = false;

    // Intersection test
    if (config.useBVH && bvhRoot)
        bvhRoot->intersect(ray, hit);
    else
        for (auto* s : scene->shapes)
            s->intersect(ray, hit);

    // If no hit, return background colour
    if (!hit.hit)
        return BACKGROUND_COLOR;

    // Shade
    return shade(ray, hit, depth);
}

// Compute percentage of light visible
Vector3 computeShadowFactor(
    const Scene* scene,
    const BVHNode* bvh,
    const Vector3& origin,
    const Vector3& normal,
    const Light& light,
    const RenderConfig& config
) {
    // Determine sampling quality
    bool hardShadows = (config.shadowSamples <= 1 || light.radius <= 0.0f);
    int gridSize = static_cast<int>(std::sqrt(config.shadowSamples));
    if (gridSize < 1) gridSize = 1;
    
    Vector3 accumulatedTransmission(0.0f, 0.0f, 0.0f);
    float totalSamples = (float)(gridSize * gridSize);


    for (int y = 0; y < gridSize; ++y) {
        for (int x = 0; x < gridSize; ++x) {

            Vector3 lightPos;
            
            if (hardShadows) {
                lightPos = light.position;
            } 
            else {
                lightPos = samplePointOnLight(light, origin, x, y, gridSize);
            }

            Vector3 L = lightPos - origin;
            float dist = L.length();
            L.normalize();

            Vector3 n = normal;
            n.normalize();
            Ray shadowRay(origin + n * SHADOW_BIAS, L);

            Vector3 rayThroughput(1.0f, 1.0f, 1.0f);
            bool blocked = false;

            // Trace shadow ray
            int maxPassthrough = 10; 
            while (maxPassthrough-- > 0) {
                HitInfo h;
                if (bvh) bvh->intersect(shadowRay, h);
                else for (auto* s : scene->shapes) s->intersect(shadowRay, h);

                if (!h.hit || h.t > dist) break;
                
                // If hit glass
                if (h.shape->material.transparency > 0.0f) {
    
                    rayThroughput = rayThroughput * h.shape->material.diffuse;

                    if (rayThroughput.length() < 0.01f) {
                        blocked = true;
                        break;
                    }

                    shadowRay.origin = h.point + shadowRay.direction * SHADOW_BIAS;

                    Vector3 newL = lightPos - shadowRay.origin;
                    dist = newL.length();

                }
                                else {
                    blocked = true;
                    break;
                }
            }

            if (!blocked) {
                accumulatedTransmission = accumulatedTransmission + rayThroughput;
            }
        }
    }

    return accumulatedTransmission / totalSamples;
}


Vector3 Raytracer::shade(const Ray& ray, const HitInfo& hit, int depth) const {

    const Material& mat = hit.shape->material;
    
    Vector3 diffuseColor = mat.diffuse; 

    // Texture lookup
    if (mat.texture != nullptr) {
        
        // Calculate tiling/wrapping
        float u_tiled = hit.u - std::floor(hit.u);
        float v_tiled = hit.v - std::floor(hit.v);
        
        if (u_tiled < 0.0f) u_tiled += 1.0f;
        if (v_tiled < 0.0f) v_tiled += 1.0f;

        int texX = (int)(u_tiled * (mat.texture->width - 1));
        int texY = (int)((1.0f - v_tiled) * (mat.texture->height - 1));
        
    
        Pixel p = mat.texture->getPixel(texX, texY);
        
        // Convert Texture from sRGB to Linear Space
        auto srgbToLinear = [](float c) {
            return powf(c / 255.0f, 2.2f);
        };

        Vector3 textureColour = Vector3(
            srgbToLinear(p.r),
            srgbToLinear(p.g),
            srgbToLinear(p.b)
        );

        diffuseColor = diffuseColor * textureColour; 
    }

    if (config.noShading) {
        return diffuseColor;
    }
    
    // Ambient term
    Vector3 finalColour = diffuseColor * 0.02f;

    // Surface normal and view direction
    Vector3 N = hit.normal;
    N.normalize();
    Vector3 V = (-ray.direction);
    V.normalize();

    // Blinn-Phong
    for (const auto& light : scene->lights) {

        // Calculate shadows
        Vector3 shadowColor = computeShadowFactor(scene, bvhRoot, hit.point, N, light, config);

        // If completely in shadow, skip
        if (shadowColor.x <= 0.001f && shadowColor.y <= 0.001f && shadowColor.z <= 0.001f) {
                continue;
            }

        Vector3 L = light.position - hit.point;
        float distToLight = L.length();
        L.normalize();

        // Inverse square law
        float attenuation = 1.0f / (1.0f + distToLight * distToLight);

        // Blinn-Phong
        Vector3 H = (L + V);
        H.normalize();

        float diff = std::max(0.0f, N.dot(L));
        float spec = powf(std::max(0.0f, N.dot(H)), mat.shininess);

        Vector3 incomingLight = light.intensity * attenuation * shadowColor;

        finalColour = finalColour + (diffuseColor * diff + mat.specular * spec) * incomingLight;
    }

    // Refraction
    if (mat.transparency > 0.0f && depth < config.maxDepth) {
        
        float ior = mat.ior;
        float eta; 
        Vector3 normal = N;
        float cosi = ray.direction.dot(N);
        
        if (cosi < 0) {
            // Entering material
            cosi = -cosi;
            eta = 1.0f / ior;
        } else {
            // Exiting material
            normal = -N;
            eta = ior / 1.0f;
        }

        float k = 1.0f - eta * eta * (1.0f - cosi * cosi);
        Vector3 transmissionColor(0,0,0); 

        if (k < 0.0f) {
            Vector3 R = ray.direction - normal * (2.0f * ray.direction.dot(normal));
            R.normalize();
            
            Ray internalRay(hit.point + normal * REFLECTION_BIAS, R); 
            transmissionColor = traceRay(internalRay, depth + 1);
        } 
        else {
            // Total Internal Reflection
            Vector3 refractDir = ray.direction * eta + normal * (eta * cosi - sqrtf(k));
            refractDir.normalize();

            Ray refractedRay(hit.point + refractDir * REFLECTION_BIAS, refractDir);
            transmissionColor = traceRay(refractedRay, depth + 1);
        }

        finalColour = (finalColour * (1.0f - mat.transparency)) + (transmissionColor * mat.transparency);
    }

    // Reflection
    if (mat.reflectivity > 0.0f && depth < config.maxDepth) {
        
        Vector3 R = ray.direction - N * (2.0f * ray.direction.dot(N));
        R.normalize();

        // Check if Glossy or Perfect Mirror
        if (mat.roughness <= 0.001f || config.glossySamples <= 1) {

            Ray reflectedRay(hit.point + N * REFLECTION_BIAS, R);
            Vector3 reflectedColor = traceRay(reflectedRay, depth + 1);
            finalColour = (finalColour * (1.0f - mat.reflectivity)) + (reflectedColor * mat.reflectivity);
        }
        else {
            // Glossy Reflections
            Vector3 accumulatedReflection(0, 0, 0);
            
            int samples = config.glossySamples;
            if (depth > 0) samples = std::max(1, samples / 2);

            int gridSize = static_cast<int>(std::sqrt(samples));
            if (gridSize < 1) gridSize = 1;
            
            float validSamples = 0.0f;

            for (int y = 0; y < gridSize; ++y) {
                for (int x = 0; x < gridSize; ++x) {
                    // Jitter direction
                    Vector3 randDir = sampleUnitSphere(x, y, gridSize);
                    
                    Vector3 glossyDir = R + (randDir * mat.roughness);
                    glossyDir.normalize();

                    if (glossyDir.dot(N) < 0) {
                       continue;
                    }

                    Ray glossyRay(hit.point + N * REFLECTION_BIAS, glossyDir);
                    accumulatedReflection = accumulatedReflection + traceRay(glossyRay, depth + 1);
                    validSamples += 1.0f;
                }
            }

            if (validSamples > 0.0f) {
                Vector3 avgReflection = accumulatedReflection / validSamples;
                finalColour = (finalColour * (1.0f - mat.reflectivity)) + (avgReflection * mat.reflectivity);
            }
        }
    }

    return finalColour;
}

// Render loop
void Raytracer::render(Image& img) const {

    int width  = img.getWidth();  
    int height = img.getHeight();

    int spp = config.samplesPerPixel;
    int gridSide = static_cast<int>(std::sqrt(spp));
    if (gridSide < 1) gridSide = 1; 

    float subStep = 1.0f / gridSide;

    for (int y = 0; y < height; ++y) {

        // Progress bar
        float progress = (float)(y+1)  / (float)height;
        
        int barWidth = 60; 

        std::cout << "[";
        int pos = barWidth * progress;
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }

        std::cout << "] " << int(progress * 100.0) << " %\r" << std::flush;

        for (int x = 0; x < width; ++x) {

            Vector3 pixelColour(0, 0, 0);
            
            // Anti-Aliasing Loop
            for (int sy = 0; sy < gridSide; ++sy) {
                for (int sx = 0; sx < gridSide; ++sx) {
                    
                    float u, v;

                    if (spp == 1) {
                        // Centre pixel
                        u = x + 0.5f;
                        v = y + 0.5f;
                    } else {
                        // Stratified Jitter
                        float r1 = randomFloat(); 
                        float r2 = randomFloat();
                        u = x + (sx * subStep) + (r1 * subStep);
                        v = y + (sy * subStep) + (r2 * subStep);
                    }

                    Ray ray = camera->pixelToRay(u, v, sx, sy, gridSide);
                    pixelColour = pixelColour + traceRay(ray, 0);
                }
            }
            
            pixelColour = pixelColour / static_cast<float>(gridSide * gridSide);

            pixelColour = pixelColour * config.exposure;

            // Tone mapping 
            if (config.toneMapping == ToneMappingMode::Reinhard) {
                pixelColour = reinhardToneMapping(pixelColour);
            } else if (config.toneMapping == ToneMappingMode::ACES) {
                pixelColour = acesToneMapping(pixelColour);
            } 

            // Gamma Correction
            float invGamma = 1.0f / 2.2f;
            pixelColour.x = powf(pixelColour.x, invGamma);
            pixelColour.y = powf(pixelColour.y, invGamma);
            pixelColour.z = powf(pixelColour.z, invGamma);
            
           
            img.setPixel(x, y, Pixel(
                static_cast<unsigned char>(std::clamp(pixelColour.x * 255.0f, 0.0f, 255.0f)),
                static_cast<unsigned char>(std::clamp(pixelColour.y * 255.0f, 0.0f, 255.0f)),
                static_cast<unsigned char>(std::clamp(pixelColour.z * 255.0f, 0.0f, 255.0f))
            ));
        }
    }

}