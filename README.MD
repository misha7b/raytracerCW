# CGR 2025 Raytracer

Export blend files to a `.txt` file that the raytracer can read with the `export.py` script. 

Implemented a Whitted-style raytracer, soft shadows, glossy reflections, lens effects and triangle meshes.

## Build
```bash
make raytracer
```

## Run
```bash
./raytracer -i scene.txt -o output.ppm
```

## Command-Line Options
```bash
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
```

![Glass render](Output/glass.png)
![Glass render](Output/glass2.png)
![Glass render](Output/glass3.png)