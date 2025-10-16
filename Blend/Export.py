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
            
            camera = obj.data
            location = obj.location
            gaze_vector = obj.matrix_world.to_3x3() @ mathutils.Vector((0.0, 0.0, -1.0))
            focal_length = camera.lens
            sensor_width = camera.sensor_width
            sensor_height = camera.sensor_height
            film_resolution_x = resolution_x
            film_resolution_y = resolution_y
            
            file.write("BEGIN_CAMERA\n")
            file.write(f"CAMERA {obj.name}\n")
            file.write(f"Location {location.x:.6f} {location.y:.6f} {location.z:.6f}\n")
            file.write(f"Gaze {gaze_vector.x:.6f} {gaze_vector.y:.6f} {gaze_vector.z:.6f}\n")
            file.write(f"FocalLength {focal_length:.6f}\n")
            file.write(f"SensorWidth {sensor_width:.6f}\n")
            file.write(f"SensorHeight {sensor_height:.6f}\n")
            file.write(f"ResolutionX {resolution_x}\n")
            file.write(f"ResolutionY {resolution_y}\n")
            file.write("END_CAMERA\n\n")
            
        elif obj.type == "LIGHT" and obj.data.type == "POINT":
            
            light = obj.data
            location = obj.location
            radiant_intensity = light.energy
            
            file.write("BEGIN_LIGHT\n")
            file.write(f"LIGHT {obj.name}\n")
            file.write(f"Location {location.x:.6f} {location.y:.6f} {location.z:.6f}\n")
            file.write(f"Intensity {radiant_intensity:.6f}\n")
            file.write("END_LIGHT\n\n")
            
       elif (obj.type == "MESH" and "sphere" in obj.name.lower()) or obj.type == "META":
           
            location = obj.location
            radius = obj.scale.x
            
            file.write("BEGIN_SPHERE\n")
            file.write(f"SPHERE {obj.name}\n")
            file.write(f"Location {location.x:.6f} {location.y:.6f} {location.z:.6f}\n")
            file.write(f"Radius {radius:.6f}\n")
            file.write("END_SPHERE\n\n")
            
        elif obj.type == "MESH" and "cube" in obj.name.lower():
                    
            location = obj.location
            rotation = obj.rotation_euler
            scale_1d = obj.scale.x 
            
            file.write("BEGIN_CUBE\n")
            file.write(f"CUBE {obj.name}\n")
            file.write(f"Translation {location.x:.6f} {location.y:.6f} {location.z:.6f}\n")
            file.write(f"Rotation {rotation.x:.6f} {rotation.y:.6f} {rotation.z:.6f}\n")
            file.write(f"Scale {scale_1d:.6f}\n")
            file.write("END_CUBE\n\n")
            
        elif obj.type == "MESH" and "plane" in obj.name.lower():
                
            mesh = obj.data        

            file.write("BEGIN_PLANE\n")
            file.write(f"PLANE {obj.name}\n")
            for vertex in mesh.vertices:
                world_coord = obj.matrix_world @ vertex.co
                file.write(f"Vertex {world_coord.x:.6f} {world_coord.y:.6f} {world_coord.z:.6f}\n")
            file.write("END_PLANE\n\n")


    
    

