import bpy
import os
import mathutils

# Get file info
filepath = bpy.data.filepath
name = os.path.splitext(os.path.basename(filepath))[0]

# Setup output path
blend_dir = os.path.dirname(filepath)
parent_dir = os.path.dirname(blend_dir)
ascii_dir = os.path.join(parent_dir, "ASCII")

output_path = os.path.join(ascii_dir, f"{name}.txt")

def write_material(file, obj):

    # defaults
    diffuse = [0.8, 0.8, 0.8]
    specular = [1.0, 1.0, 1.0]
    shininess = 32.0
    roughness = 0.0
    reflectivity = 0.0
    transparency = 0.0
    ior = 1.0
    texture_filename = "none"

    if obj.data.materials:
        mat = obj.data.materials[0]

        if mat.use_nodes and mat.node_tree:
            for node in mat.node_tree.nodes:

                if node.type == 'BSDF_DIFFUSE':
                    c = node.inputs['Color'].default_value
                    diffuse = [float(c[0]), float(c[1]), float(c[2])]

                elif node.type == 'BSDF_GLOSSY':
                    c = node.inputs['Color'].default_value
                    specular = [float(c[0]), float(c[1]), float(c[2])]

                    roughness = float(node.inputs['Roughness'].default_value)
                    r = roughness
                    
                    if r <= 0.01:
                        shininess = 1000.0
                    else:
                        shininess = (2.0 / (r*r + 0.0001)) - 2.0
                        
                elif node.type == 'MIX_SHADER':

                    fac = node.inputs['Fac'].default_value

                    s1 = node.inputs[1].links[0].from_node if node.inputs[1].links else None
                    s2 = node.inputs[2].links[0].from_node if node.inputs[2].links else None

                    if s1 and s2:
                        shader_types = {s1.type, s2.type}

                        if shader_types == {'BSDF_DIFFUSE', 'BSDF_GLOSSY'}:
                            reflectivity = fac
                            
                elif node.type == 'MIX_RGB':
    
                    if node.blend_type == 'MULTIPLY':

      
                        fac = node.inputs['Fac'].default_value

                        if not node.inputs['Color1'].is_linked:
                            c1 = node.inputs['Color1'].default_value
                            diffuse = [float(c1[0]), float(c1[1]), float(c1[2])]

                        if not node.inputs['Color2'].is_linked:
                            c2 = node.inputs['Color2'].default_value
                            diffuse = [
                                diffuse[0] * float(c2[0]),
                                diffuse[1] * float(c2[1]),
                                diffuse[2] * float(c2[2])
                            ]

                elif node.type == 'TEX_IMAGE':
                    if node.image and node.image.filepath:
                        original = os.path.basename(node.image.filepath)    
                        base, _ = os.path.splitext(original)         
                        texture_filename = base + ".ppm"                 


    if "reflectivity" in obj: reflectivity = float(obj["reflectivity"])
    if "transparency" in obj: transparency = float(obj["transparency"])
    if "ior" in obj: ior = float(obj["ior"])
    if "texture" in obj: texture_filename = obj["texture"]

    file.write(f"diffuse {diffuse[0]:.4f} {diffuse[1]:.4f} {diffuse[2]:.4f}\n")
    file.write(f"specular {specular[0]:.4f} {specular[1]:.4f} {specular[2]:.4f}\n")
    file.write(f"shininess {shininess:.4f}\n")
    file.write(f"roughness {roughness:.4f}\n")
    file.write(f"reflectivity {reflectivity:.4f}\n")
    file.write(f"transparency {transparency:.4f}\n")
    file.write(f"ior {ior:.4f}\n")
    file.write(f"texture {texture_filename}\n")

# Write to file
with open(output_path, 'w') as file:
    
    scene = bpy.context.scene
    resolution_x = scene.render.resolution_x
    resolution_y = scene.render.resolution_y
    
    # Cameras
    for obj in bpy.data.objects:
        if obj.type == "CAMERA":
            
            cam_data = obj.data
            
            gaze_vector = obj.matrix_world.to_3x3() @ mathutils.Vector((0.0, 0.0, -1.0))
            up_vector = obj.matrix_world.to_3x3() @ mathutils.Vector((0.0, 1.0, 0.0))
            
            velocity = obj.get("velocity", (0.0, 0.0, 0.0))
            aperture = obj.get("aperture", 0.0)           
            focal_d  = obj.get("focal_distance", 0.0) 
            
            file.write("BEGIN_CAMERA\n")
            file.write(f"CAMERA {obj.name}\n")
            file.write(f"location {obj.location.x:.6f} {obj.location.y:.6f} {obj.location.z:.6f}\n")
            file.write(f"gaze {gaze_vector.x:.6f} {gaze_vector.y:.6f} {gaze_vector.z:.6f}\n")
            file.write(f"up {up_vector.x:.6f} {up_vector.y:.6f} {up_vector.z:.6f}\n")
            file.write(f"focal_length {cam_data.lens:.6f}\n")
            file.write(f"sensor_size {cam_data.sensor_width:.6f} {cam_data.sensor_height:.6f}\n")
            file.write(f"resolution {scene.render.resolution_x} {scene.render.resolution_y}\n")
            file.write(f"velocity {velocity[0]} {velocity[1]} {velocity[2]}\n")
            file.write(f"aperture {aperture}\n")
            file.write(f"focal_distance {focal_d}\n")
            file.write("END_CAMERA\n\n")
            
        elif obj.type == "LIGHT":
            
            light = obj.data
            location = obj.location
            radiant_intensity = light.energy
            
            file.write("BEGIN_LIGHT\n")
            file.write(f"LIGHT {obj.name}\n")
            file.write(f"location {location.x:.6f} {location.y:.6f} {location.z:.6f}\n")
            file.write(f"intensity {radiant_intensity:.6f}\n")
            
            if light.type == 'POINT':
                radius = light.shadow_soft_size
                file.write(f"radius {radius:.6f}\n")

            elif light.type == 'AREA':

                if light.shape in {'SQUARE', 'DISK'}:
                    radius = light.size / 2.0
                elif light.shape in {'RECTANGLE', 'ELLIPSE'}:
                    radius = (light.size + light.size_y) / 4.0
                else:
                    radius = light.size / 2.0

                file.write(f"radius {radius:.6f}\n")

            else:
                file.write("radius 0.0000\n")
                
            file.write("END_LIGHT\n\n")
                    
            
        elif (obj.type == "MESH" and "sphere" in obj.name.lower()) or obj.type == "META":
           
            loc = obj.location
            rot = obj.rotation_euler
            scale = obj.scale 
            
            file.write("BEGIN_SPHERE\n")
            file.write(f"SPHERE {obj.name}\n")
            file.write(f"translation {loc.x:.6f} {loc.y:.6f} {loc.z:.6f}\n")
            file.write(f"rotation {rot.x:.6f} {rot.y:.6f} {rot.z:.6f}\n")
            file.write(f"scale {scale.x:.6f} {scale.y:.6f} {scale.z:.6f}\n")
            
            write_material(file, obj)
            
            file.write("END_SPHERE\n\n")
            
        elif obj.type == "MESH" and "cube" in obj.name.lower():
                    
            loc = obj.location
            rot = obj.rotation_euler
            scale = obj.scale
            
            file.write("BEGIN_CUBE\n")
            file.write(f"CUBE {obj.name}\n")
            file.write(f"translation {loc.x:.6f} {loc.y:.6f} {loc.z:.6f}\n")
            file.write(f"rotation {rot.x:.6f} {rot.y:.6f} {rot.z:.6f}\n")
            file.write(f"scale {scale.x:.6f} {scale.y:.6f} {scale.z:.6f}\n")
            
            write_material(file, obj)
            
            file.write("END_CUBE\n\n")
            
        elif obj.type == "MESH" and "plane" in obj.name.lower():
                
            mesh = obj.data        

            file.write("BEGIN_PLANE\n")
            file.write(f"PLANE {obj.name}\n")
            for vertex in mesh.vertices:
                world_coord = obj.matrix_world @ vertex.co
                file.write(f"vertex {world_coord.x:.6f} {world_coord.y:.6f} {world_coord.z:.6f}\n")
            
            write_material(file, obj)
                
            file.write("END_PLANE\n\n")
            
        elif obj.type == "MESH" and "obj" in obj.name.lower():

            loc = obj.location
            rot = obj.rotation_euler

            scale = obj.scale.x 
            
            mesh_filename = obj.name + ".obj"
            
            file.write("BEGIN_MESH\n")
            file.write(f"file {mesh_filename}\n")
            file.write(f"translation {loc.x:.6f} {loc.y:.6f} {loc.z:.6f}\n")
            file.write(f"rotation {rot.x:.6f} {rot.y:.6f} {rot.z:.6f}\n")
            file.write(f"scale {scale:.6f}\n")
            
            write_material(file, obj)
            
            file.write("END_MESH\n\n")


    
    

