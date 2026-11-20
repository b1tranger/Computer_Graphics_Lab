CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -I./include

SRCS = ./src/main.cpp \
       ./src/glad.c \
       ./src/SphericalMesh.cpp \
       ./src/GeodesicEdges.cpp \
       ./src/SceneRenderer.cpp \
       ./src/RayPicker.cpp

# Windows target
win:
	$(CXX) $(CXXFLAGS) $(SRCS) -o ./build/main.exe -L./lib -lglfw3 -lopengl32 -lgdi32
	@echo Build completed: ./build/main.exe

run-win: win
	./build/main.exe

# Linux target
linux:
	$(CXX) $(CXXFLAGS) $(SRCS) -o ./build/main -L./lib -lglfw -lGL -lXrandr -lX11 -lrt -ldl
	@echo Build completed: ./build/main

run-linux: linux
	./build/main

clean:
	rm -f ./build/main.exe ./build/main
