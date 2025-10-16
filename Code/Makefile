CXX = g++
CXXFLAGS = -std=c++11 -Wall -I.

all: Tests/test_camera Tests/test_image

Tests/test_camera: Tests/test_camera.cpp camera.cpp camera.h
	$(CXX) $(CXXFLAGS) Tests/test_camera.cpp camera.cpp -o Tests/test_camera

Tests/test_image: Tests/test_image.cpp image.cpp image.h
	$(CXX) $(CXXFLAGS) Tests/test_image.cpp image.cpp -o Tests/test_image

clean:
	rm -f Tests/test_camera Tests/test_image