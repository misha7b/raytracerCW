#include "raytracer.h"
#include <algorithm>
#include <cmath>
#include <iostream>

const Vector3 BACKGROUND_COLOR(0.2f, 0.2f, 0.2f);
const float SHADOW_BIAS = 0.001f;


Vector3 Raytracer::shade(const Ray& ray, const HitInfo& hit, int depth) const {
    const Material& mat = hit.shape->material;

    Vector3 localColour = mat.diffuse * 0.1f;

    // normal
    Vector3 N = hit.normal;
    N.normalize();

    // view direction
    Vector3 V = -ray.direction;
    V.normalize();

    // loop over lights
    for (const auto& light : scene->lights) {

        Vector3 L = light.position - hit.point;
        float distToLight = L.length();
        L.normalize();

        Ray shadowRay(hit.point + N * SHADOW_BIAS, L);

        bool blocked = false;
        HitInfo h;
        h.hit = false;

        for (auto* s : scene->shapes) {
            if (s->intersect(shadowRay, h)) {
                if (h.t < distToLight) {
                    blocked = true;
                    break;
                }
            }
        }

        if (blocked) {
            continue; 
        }

        // inverse square law
        float attenuation = 1.0f / (1.0f + distToLight * distToLight);
        Vector3 intensity = light.intensity * attenuation;

        float diff = std::max(0.0f, N.dot(L));

        // Blinn-Phong
        Vector3 H = (L + V);
        H.normalize();
        float spec = std::pow(std::max(0.0f, N.dot(H)), mat.shininess);

        localColour = localColour + (mat.diffuse * diff + mat.specular * spec) * intensity;
    }   
    return localColour; 
}

Vector3 Raytracer::traceRay(const Ray& ray, int depth) const {
    HitInfo hit;
    hit.hit = false;
    
    for (auto* s : scene->shapes) {
        s->intersect(ray, hit);
    }

    if (!hit.hit) {
        return BACKGROUND_COLOR; 
    }

    return shade(ray, hit, depth);
}


void Raytracer::render(Image& img) const {
    int width  = img.getWidth();  
    int height = img.getHeight();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {

            float u = x + 0.5f;
            float v = y + 0.5f;

            Ray ray = camera->pixelToRay(u, v);

            Vector3 pixelColour = traceRay(ray, 0);

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