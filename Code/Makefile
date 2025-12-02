CXX = g++
CXXFLAGS = -std=c++17 -Wall -I.

SRCS = main.cpp raytracer.cpp camera.cpp image.cpp scene.cpp \
       shapes/cube.cpp shapes/sphere.cpp shapes/plane.cpp

HEADERS = raytracer.h 
          
all: raytracer Tests/test_camera Tests/test_image

raytracer: $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS)

Tests/test_camera: Tests/test_camera.cpp camera.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

Tests/test_image: Tests/test_image.cpp image.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f raytracer Tests/test_camera Tests/test_image
