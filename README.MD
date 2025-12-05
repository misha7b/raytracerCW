# CGR 2025 Raytracer — README

A modular Whitted-style raytracer written in C++17 for the CGR 2025 coursework.

## Build
make raytracer

## Run
./raytracer -i scene.txt -o output.ppm

## Command-Line Options
- -i <file> — input scene
- -o <file> — output PPM
- -w, -h — override resolution
- -spp <N> — antialiasing samples
- -d <N> — recursion depth
- -no-bvh — disable BVH
- -no-shading — disable shading
- --shadow-samples <N> — soft shadows
- --glossy-samples <N> — glossy reflections
- -exposure <x> — exposure multiplier
- -tonemap <none|reinhard|aces> — tone mapping
- --help — list options

## Features
- Camera system, PPM I/O
- Sphere, cube, plane, triangle intersection
- BVH acceleration
- Blinn–Phong shading, reflections, refractions
- Stratified supersampling AA
- Texture mapping
- Soft shadows & glossy reflections
- Motion blur & depth of field
- ACES/Reinhard tone mapping & gamma correction

![Glass render](Output/glass.png)
