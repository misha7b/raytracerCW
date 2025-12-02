#include "raytracer.h"
#include <algorithm>
#include <cmath>
#include <iostream>

const Vector3 BACKGROUND_COLOR(0.2f, 0.2f, 0.2f);
const float SHADOW_BIAS = 0.001f;
const float REFLECTION_BIAS = 0.01f;

float randomFloat() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

Vector3 randomInUnitSphere() {
    Vector3 p;
    do {
        // Generate random vector in cube [-1, 1]
        p = Vector3(randomFloat(), randomFloat(), randomFloat()) * 2.0f - Vector3(1, 1, 1);
    } while (pow(p.length(), 2) >= 1.0f); // Reject if outside sphere
    return p;
}

#ifndef M_PI
#define M_PI 3.1415
#endif

Vector3 samplePointOnLight(const Light& light) {
    // if radius 0, point light
    if (light.radius <= 0.0f) return light.position;

    float r = std::sqrt(randomFloat()) * light.radius;
    float theta = randomFloat() * 2.0f * M_PI;

    float x = r * std::cos(theta);
    float y = r * std::sin(theta);
    float z = (randomFloat() - 0.5f) * 2.0f * light.radius; 

    return light.position + Vector3(x, y, z);
}

float computeShadowFactor(
    const Scene* scene, 
    const BVHNode* bvhRoot, 
    const RenderConfig& config,
    const Vector3& point, 
    const Vector3& normal, 
    const Light& light
) {
    int samples = config.shadowSamples;
    if (samples < 1) samples = 1;

    // hard shadows
    if (samples == 1 || light.radius <= 0.0f) {
        Vector3 L = light.position - point;
        float dist = L.length();
        L.normalize();
        
        Ray shadowRay(point + normal * SHADOW_BIAS, L);
        HitInfo h; h.hit = false;

        // Use BVH if enabled
        if (config.useBVH && bvhRoot) {
            bvhRoot->intersect(shadowRay, h);
            if (h.hit && h.t < dist) {
                if (h.shape->material.transparency <= 0.0f) return 0.0f; // Blocked
            }
        } else {

            for (auto* s : scene->shapes) {
                if (s->intersect(shadowRay, h) && h.t < dist) {
                    if (s->material.transparency > 0.0f) continue; 
                    return 0.0f;
                }
            }
        }
        return 1.0f; // Visible
    }

    // soft shadows
    int unblockedCount = 0;

    for (int i = 0; i < samples; i++) {
        Vector3 lightPoint = samplePointOnLight(light);
        Vector3 L = lightPoint - point;
        float dist = L.length();
        L.normalize();

        Ray shadowRay(point + normal * SHADOW_BIAS, L);
        HitInfo h; h.hit = false;
        bool blocked = false;

        if (config.useBVH && bvhRoot) {
            bvhRoot->intersect(shadowRay, h);
            if (h.hit && h.t < dist) {
                if (h.shape->material.transparency <= 0.0f) {
                    blocked = true;
                }
            }
        } else {
            for (auto* s : scene->shapes) {
                if (s->intersect(shadowRay, h) && h.t < dist) {
                    if (s->material.transparency > 0.0f) continue;
                    blocked = true;
                    break;
                }
            }
        }

        if (!blocked) unblockedCount++;
    }

    return static_cast<float>(unblockedCount) / static_cast<float>(samples);
}

Vector3 Raytracer::shade(const Ray& ray, const HitInfo& hit, int depth) const {
    const Material& mat = hit.shape->material;

    Vector3 diffuseColor = mat.diffuse; 
    
    if (mat.texture != nullptr) {
        // wrapping/tiling
        float u_tiled = hit.u - std::floor(hit.u);
        float v_tiled = hit.v - std::floor(hit.v);
        
        if (u_tiled < 0.0f) u_tiled += 1.0f;
        if (v_tiled < 0.0f) v_tiled += 1.0f;

        int texX = (int)(u_tiled * (mat.texture->width - 1));
        int texY = (int)((1.0f - v_tiled) * (mat.texture->height - 1));
    
        Pixel p = mat.texture->getPixel(texX, texY);
        
        //  sRGB to Linear 
        auto srgbToLinear = [](float c) { return std::pow(c / 255.0f, 2.2f); };

        diffuseColor = Vector3(
            srgbToLinear(p.r),
            srgbToLinear(p.g),
            srgbToLinear(p.b)
        );
    }

    // Initialize Local Color with Ambient
    Vector3 localColour = diffuseColor * 0.1f;

    // normal
    Vector3 N = hit.normal;
    N.normalize();

    // view direction
    Vector3 V = -ray.direction;
    V.normalize();

    // loop over lights
    for (const auto& light : scene->lights) {

        int shadowSamples = config.shadowSamples; 
        if (shadowSamples < 1) shadowSamples = 1;

        float shadowFactor = computeShadowFactor(scene, bvhRoot, config, hit.point, N, light);

        if (shadowFactor <= 0.0f) {
            continue;
        }

        Vector3 L = light.position - hit.point;
        float distToLight = L.length();
        L.normalize();

        // inverse square law
        float attenuation = 1.0f / (1.0f + distToLight * distToLight);
        Vector3 intensity = light.intensity * attenuation * shadowFactor;

        float diff = std::max(0.0f, N.dot(L));

        // Blinn-Phong
        Vector3 H = (L + V);
        H.normalize();
        float spec = std::pow(std::max(0.0f, N.dot(H)), mat.shininess);

        localColour = localColour + (diffuseColor * diff + mat.specular * spec) * intensity;

        
    }   
    
    Vector3 finalColour = localColour;

    // if material transparent and below recursion limit
    if (mat.transparency > 0.0f && depth < config.maxDepth) {
        
        float ior = mat.ior;
        float eta; 
        Vector3 normal = N;
        float cosi = ray.direction.dot(N);
        
        if (cosi < 0) {
            // Entering 
            cosi = -cosi;
            eta = 1.0f / ior;
        } else {
            // Exiting 
            normal = -N;
            eta = ior / 1.0f;
        }

        float k = 1.0f - eta * eta * (1.0f - cosi * cosi);

        if (k < 0.0f) {
            Vector3 R = ray.direction - normal * (2.0f * ray.direction.dot(normal));
            R.normalize();
            
            Ray internalRay(hit.point + R * REFLECTION_BIAS, R); 
        
            finalColour = traceRay(internalRay, depth + 1);
        } 
        else {
            // Snell's Law
            Vector3 refractDir = ray.direction * eta + normal * (eta * cosi - sqrtf(k));
            refractDir.normalize();

            Ray refractedRay(hit.point + refractDir * REFLECTION_BIAS, refractDir);
            Vector3 refractedColor = traceRay(refractedRay, depth + 1);
            
            finalColour = (finalColour * (1.0f - mat.transparency)) + (refractedColor * mat.transparency);
        }
    }

    // if material reflective and below recursion limit
    if (mat.reflectivity > 0.0f && depth < config.maxDepth) {

        Vector3 perfectR = ray.direction - N * (2.0f * ray.direction.dot(N));

        Vector3 reflectedAccum(0, 0, 0);
        
        int samples = (config.glossySamples < 1) ? 1 : config.glossySamples;

        for (int i = 0; i < samples; i++) {
            
            Vector3 R = perfectR;

            if (mat.roughness > 0.0f) {
                R = R + (randomInUnitSphere() * mat.roughness);
            }
            
            R.normalize();

            Ray reflectedRay(hit.point + R * REFLECTION_BIAS, R);
            
            reflectedAccum = reflectedAccum + traceRay(reflectedRay, depth + 1);
        }

        Vector3 reflectedColour = reflectedAccum / static_cast<float>(samples);

        finalColour = (finalColour * (1.0f - mat.reflectivity)) + (reflectedColour * mat.reflectivity);
    
    }

    return finalColour;
}

Vector3 Raytracer::traceRay(const Ray& ray, int depth) const {
    HitInfo hit;
    hit.hit = false;
    
    if (config.useBVH && bvhRoot) {
        bvhRoot->intersect(ray, hit);
    } else {
        for (auto* s : scene->shapes) {
            s->intersect(ray, hit);
        }
    }

    if (!hit.hit) {
        return BACKGROUND_COLOR; 
    }

    return shade(ray, hit, depth);
}


void Raytracer::render(Image& img) const {
    int width  = img.getWidth();  
    int height = img.getHeight();

    int spp = config.samplesPerPixel;

    int gridSide = static_cast<int>(std::sqrt(spp));
    if (gridSide < 1) gridSide = 1;

    float totalSamples = static_cast<float>(gridSide * gridSide);
    float subStep = 1.0f / gridSide;

    float exposure = 0.6f; 

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {

            Vector3 pixelAccumulator(0, 0, 0);

            for (int sy = 0; sy < gridSide; ++sy) {
                for (int sx = 0; sx < gridSide; ++sx) {
                    
                    float u, v;

                    if (gridSide == 1) {
                        // centre sampling
                        u = x + 0.5f;
                        v = y + 0.5f;
                    } else {
                        // jittered sampling
                        float r1 = randomFloat(); 
                        float r2 = randomFloat();
                        
                        u = x + (sx * subStep) + (r1 * subStep);
                        v = y + (sy * subStep) + (r2 * subStep);
                    }

                    Ray ray = camera->pixelToRay(u, v);
                    pixelAccumulator = pixelAccumulator + traceRay(ray, 0);
                }
            }
            
            // Average the samples
            Vector3 pixelColour = pixelAccumulator / totalSamples;

            pixelColour = pixelColour * exposure;

            // clamp
            pixelColour.x = std::clamp(pixelColour.x, 0.0f, 1.0f);
            pixelColour.y = std::clamp(pixelColour.y, 0.0f, 1.0f);
            pixelColour.z = std::clamp(pixelColour.z, 0.0f, 1.0f);

            img.setPixel(x, y, Pixel(
                static_cast<unsigned char>(pixelColour.x * 255.0f),
                static_cast<unsigned char>(pixelColour.y * 255.0f),
                static_cast<unsigned char>(pixelColour.z * 255.0f)
            ));
        }
    }

    std::cout << "Render complete" << std::endl;
}
