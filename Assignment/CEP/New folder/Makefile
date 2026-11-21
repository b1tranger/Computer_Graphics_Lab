CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -I./include

SRCS = ./src/main.cpp \
       ./src/glad.c \
       ./src/SphericalMesh.cpp \
       ./src/GeodesicEdges.cpp \
       ./src/SceneRenderer.cpp \
       ./src/RayPicker.cpp

# Windows target: compile and run
win:
	$(CXX) $(CXXFLAGS) $(SRCS) -o ./build/main.exe -L./lib -lglfw3 -lopengl32 -lgdi32
	./build/main.exe

# Linux target: compile and run
linux:
	$(CXX) $(CXXFLAGS) $(SRCS) -o ./build/main -L./lib -lglfw -lGL -lXrandr -lX11 -lrt -ldl
	./build/main

clean:
	rm -f ./build/main.exe ./build/main
