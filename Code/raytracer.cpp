#include "raytracer.h"
#include <algorithm>
#include <cmath>
#include <iostream>

const float SHADOW_BIAS = 0.001;
const float REFLECTION_BIAS = 0.001;
const Vector3 BACKGROUND_COLOR(0.2f, 0.2f, 0.2f);

// helper for antialiasing
float randomFloat() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

Vector3 samplePointOnLight(const Light& light) {
    if (light.radius <= 0.0f)
        return light.position;  // point light fallback

    float r = sqrt(randomFloat()) * light.radius;
    float theta = randomFloat() * 2.0f * M_PI;

    float x = r * cos(theta);
    float y = r * sin(theta);

    return light.position + Vector3(x, y, 0);
}


Vector3 Raytracer::traceRay(const Ray& ray, int depth) const {
    HitInfo hit;
    hit.hit = false;

    if (config.useBVH && bvhRoot)
        bvhRoot->intersect(ray, hit);
    else
        for (auto* s : scene->shapes)
            s->intersect(ray, hit);

    if (!hit.hit)
        return BACKGROUND_COLOR; // background colour

    // if hit, compute shading
    return shade(ray, hit, depth);
}

float computeShadowFactor(
    const Scene* scene,
    const BVHNode* bvh,
    const Vector3& origin,
    const Vector3& normal,
    const Light& light,
    const RenderConfig& config
){
    int N = config.shadowSamples;
    if (N <= 1 || light.radius <= 0.0f) {
        // Hard shadow fallback
        Vector3 L = light.position - origin;
        float dist = L.length();
        L.normalize();

        Ray shadowRay(origin + normal * SHADOW_BIAS, L);
        HitInfo h;
        h.hit = false;

        if (bvh)
            bvh->intersect(shadowRay, h);
        else
            for (auto* s : scene->shapes)
                s->intersect(shadowRay, h);

        return (h.hit && h.t < dist) ? 0.0f : 1.0f;
    }

    // Soft shadow
    int unblocked = 0;

    for (int i = 0; i < N; i++) {
        Vector3 lightSample = samplePointOnLight(light);

        Vector3 L = lightSample - origin;
        float dist = L.length();
        L.normalize();

        Ray shadowRay(origin + normal * SHADOW_BIAS, L);

        HitInfo h;
        h.hit = false;

        bool blocked = false;

        if (bvh) {
            bvh->intersect(shadowRay, h);
            blocked = (h.hit && h.t < dist);
            
            if (blocked && h.shape->material.transparency > 0.0f) {
                blocked = false;
            }

        } else {
            for (auto* s : scene->shapes) {
                if (s->intersect(shadowRay, h) && h.t < dist) {
                    
                    if (s->material.transparency > 0.0f) {
                        continue;
                    }

                    blocked = true;
                    break;
                }
            }
        }

        if (!blocked)
            unblocked++;
        }

    return float(unblocked) / float(N);
}



Vector3 Raytracer::shade(const Ray& ray, const HitInfo& hit, int depth) const {

    const Material& mat = hit.shape->material;
    
    Vector3 diffuseColor = mat.diffuse; 

    // Texture lookup
    if (mat.texture != nullptr) {
        
        // calculate tiling/wrapping
        float u_tiled = hit.u - std::floor(hit.u);
        float v_tiled = hit.v - std::floor(hit.v);
        
        if (u_tiled < 0.0f) u_tiled += 1.0f;
        if (v_tiled < 0.0f) v_tiled += 1.0f;

        int texX = (int)(u_tiled * (mat.texture->width - 1));
        int texY = (int)((1.0f - v_tiled) * (mat.texture->height - 1));
        
    
        Pixel p = mat.texture->getPixel(texX, texY);
        
        auto srgbToLinear = [](float c) {
            return powf(c / 255.0f, 2.2f);
        };

        diffuseColor = Vector3(
            srgbToLinear(p.r),
            srgbToLinear(p.g),
            srgbToLinear(p.b)
        );
    }
    
    Vector3 localColour = diffuseColor * 0.1f;

    // Surface normal and view direction
    Vector3 N = hit.normal;
    N.normalize();
    Vector3 V = (-ray.direction);
    V.normalize();


    for (const auto& light : scene->lights) {
        
        float shadow = computeShadowFactor(scene, bvhRoot, hit.point, N, light, config);
        if (shadow <= 0.0f) {
            continue;
        }
        
        Vector3 L = light.position - hit.point;
        float distToLight = L.length();
        L.normalize();

        float attenuation = 1.0f / (1.0f + distToLight * distToLight);

        // blinn-phong
        Vector3 H = (L + V);
        H.normalize();

        float diff = std::max(0.0f, N.dot(L));
        float spec = powf(std::max(0.0f, N.dot(H)), mat.shininess);

        diff *= shadow;
        spec *= shadow;

        Vector3 intensity = light.intensity * attenuation;

        localColour = localColour + (diffuseColor * diff + mat.specular * spec) * intensity;
    }

    Vector3 finalColour = localColour;

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
            
            Ray internalRay(hit.point + normal * REFLECTION_BIAS, R); 
        
            finalColour = traceRay(internalRay, depth + 1);
        } 
        else {
            Vector3 refractDir = ray.direction * eta + normal * (eta * cosi - sqrtf(k));
            refractDir.normalize();

            Ray refractedRay(hit.point + refractDir * REFLECTION_BIAS, refractDir);
            
            Vector3 refractedColor = traceRay(refractedRay, depth + 1);
            
            finalColour = (finalColour * (1.0f - mat.transparency)) + (refractedColor * mat.transparency);
        }
    }

    if (mat.reflectivity > 0.0f && depth < config.maxDepth) {
        
        Vector3 R = ray.direction - N * (2.0f * ray.direction.dot(N));
        R.normalize();

        Ray reflectedRay(hit.point + N * REFLECTION_BIAS, R);
        Vector3 reflectedColor = traceRay(reflectedRay, depth + 1);
    
        finalColour = (finalColour * (1.0f - mat.reflectivity)) + (reflectedColor * mat.reflectivity);
    }

    return finalColour;
}

void Raytracer::render(Image& img) const {

    int width  = img.getWidth();  
    int height = img.getHeight();

    int spp = config.samplesPerPixel;
    int gridSide = static_cast<int>(std::sqrt(spp));
    if (gridSide < 1) gridSide = 1; 

    float subStep = 1.0f / gridSide;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {

            Vector3 pixelColour(0, 0, 0);
            
            
            for (int sy = 0; sy < gridSide; ++sy) {
                for (int sx = 0; sx < gridSide; ++sx) {
                    
                    float u, v;

                    if (spp == 1) {
                        // Center pixel
                        u = x + 0.5f;
                        v = y + 0.5f;
                    } else {
                        // Stratified Jitter
                        float r1 = randomFloat(); 
                        float r2 = randomFloat();
                        u = x + (sx * subStep) + (r1 * subStep);
                        v = y + (sy * subStep) + (r2 * subStep);
                    }

                    Ray ray = camera->pixelToRay(u, v);
                    pixelColour = pixelColour + traceRay(ray, 0);
                }
            }
            
            pixelColour = pixelColour / static_cast<float>(gridSide * gridSide);


            pixelColour.x = std::max(pixelColour.x, 0.0f);
            pixelColour.y = std::max(pixelColour.y, 0.0f);
            pixelColour.z = std::max(pixelColour.z, 0.0f);
            
            pixelColour = pixelColour / (pixelColour + Vector3(1,1,1));

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

    std::cout << "Render complete" << std::endl;
}