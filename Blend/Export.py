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
            
            file.write("BEGIN_CAMERA\n")
            file.write(f"CAMERA {obj.name}\n")
            file.write(f"location {obj.location.x:.6f} {obj.location.y:.6f} {obj.location.z:.6f}\n")
            file.write(f"gaze {gaze_vector.x:.6f} {gaze_vector.y:.6f} {gaze_vector.z:.6f}\n")
            file.write(f"up {up_vector.x:.6f} {up_vector.y:.6f} {up_vector.z:.6f}\n")
            file.write(f"focal_length {cam_data.lens:.6f}\n")
            file.write(f"sensor_size {cam_data.sensor_width:.6f} {cam_data.sensor_height:.6f}\n")
            file.write(f"resolution {scene.render.resolution_x} {scene.render.resolution_y}\n")
            file.write("END_CAMERA\n\n")
            
        elif obj.type == "LIGHT" and obj.data.type == "POINT":
            
            light = obj.data
            location = obj.location
            radiant_intensity = light.energy
            
            file.write("BEGIN_LIGHT\n")
            file.write(f"LIGHT {obj.name}\n")
            file.write(f"location {location.x:.6f} {location.y:.6f} {location.z:.6f}\n")
            file.write(f"intensity {radiant_intensity:.6f}\n")
            file.write("END_LIGHT\n\n")
            
        elif (obj.type == "MESH" and "sphere" in obj.name.lower()) or obj.type == "META":
           
            loc = obj.location
            radius = obj.scale.x 
            
            file.write("BEGIN_SPHERE\n")
            file.write(f"SPHERE {obj.name}\n")
            file.write(f"location {loc.x:.6f} {loc.y:.6f} {loc.z:.6f}\n")
            file.write(f"radius {radius:.6f}\n")
            file.write("END_SPHERE\n\n")
            
        elif obj.type == "MESH" and "cube" in obj.name.lower():
                    
            loc = obj.location
            rot = obj.rotation_euler
            scale = obj.scale.x
            
            file.write("BEGIN_CUBE\n")
            file.write(f"CUBE {obj.name}\n")
            file.write(f"translation {loc.x:.6f} {loc.y:.6f} {loc.z:.6f}\n")
            file.write(f"rotation {rot.x:.6f} {rot.y:.6f} {rot.z:.6f}\n")
            file.write(f"scale {scale:.6f}\n")
            file.write("END_CUBE\n\n")
            
        elif obj.type == "MESH" and "plane" in obj.name.lower():
                
            mesh = obj.data        

            file.write("BEGIN_PLANE\n")
            file.write(f"PLANE {obj.name}\n")
            for vertex in mesh.vertices:
                world_coord = obj.matrix_world @ vertex.co
                file.write(f"vertex {world_coord.x:.6f} {world_coord.y:.6f} {world_coord.z:.6f}\n")
            file.write("END_PLANE\n\n")


    
    

