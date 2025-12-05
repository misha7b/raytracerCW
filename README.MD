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

![Glass render](Output/glass.png)
![Glass render](Output/glass2.png)
![Glass render](Output/glass3.png)