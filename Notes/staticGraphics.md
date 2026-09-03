# Comprehensive Guide to Static Graphics in OpenGL (Modern & Legacy)

In computer graphics, **static graphics** refers to rendering visual geometry, scenes, and compositions whose shapes, colors, and vertex positions remain constant across rendering cycles or are computed once and displayed continuously.

This guide covers the core theoretical foundations, mathematical concepts, implementation architectures (both **Modern OpenGL Core Profile** using GLFW/GLAD and **Legacy OpenGL** using FreeGLUT/GLUT), GLSL shader programming, and equivalent foundational C++ implementations.

---

## 1. Core Concepts & The Graphics Pipeline

### The Programmable Rendering Pipeline
In modern OpenGL (3.3+ Core Profile), the fixed-function pipeline is deprecated in favor of a programmable pipeline controlled by **GLSL (OpenGL Shading Language)**:

```
[ Vertex Data (CPU) ]
         │
         ▼
[ Vertex Buffer Object (VBO) / VAO ]
         │
         ▼
[ Vertex Shader (Programmable) ]  <--- Transforms local coordinates to NDC (gl_Position)
         │
         ▼
[ Shape Assembly / Primitive Assembly ] <--- Groups vertices into triangles, lines, points
         │
         ▼
[ Geometry Shader (Optional) ]
         │
         ▼
[ Rasterization ] <--- Converts geometric primitives into discrete screen fragments/pixels
         │
         ▼
[ Fragment Shader (Programmable) ] <--- Calculates final pixel RGBA colors (FragColor)
         │
         ▼
[ Per-Sample Operations & Blending ] <--- Depth test, stencil test, alpha blending
         │
         ▼
[ Framebuffer / Screen Display ]
```

---

## 2. Coordinate Systems & Viewport

### Normalized Device Coordinates (NDC)
Regardless of screen resolution or aspect ratio, OpenGL clips coordinates to the Normalized Device Coordinate cube from **`-1.0` to `+1.0`** along $X, Y, \text{and } Z$:

```
                  (+1.0) Y [Top]
                     ▲
                     │
    (-1.0, +1.0)     │      (+1.0, +1.0)
         ┌───────────┼───────────┐
         │           │           │
         │           │           │
(-1.0) ──┼───────────┼───────────┼── (+1.0) X [Right]
[Left]   │        (0.0, 0.0)     │
         │         [Center]      │
         │           │           │
         └───────────┼───────────┘
    (-1.0, -1.0)     │      (+1.0, -1.0)
                     ▼
                  (-1.0) Y [Bottom]
```

* **X-axis**: $-1.0$ (left edge) to $+1.0$ (right edge)
* **Y-axis**: $-1.0$ (bottom edge) to $+1.0$ (top edge)
* **Z-axis**: $-1.0$ (near plane) to $+1.0$ (far plane) in NDC (facing into/out of screen)

### Viewport Transformation (`glViewport`)
Maps NDC $[-1.0, 1.0]$ to window pixel coordinates $(x, y, \text{width}, \text{height})$:
```cpp
glViewport(0, 0, screenWidth, screenHeight);
```

### Orthographic Projection in 2D
To map user-defined coordinates (e.g. pixel coordinates $0 \to 800$, $0 \to 600$) to NDC:
- **Legacy GLUT**: `gluOrtho2D(left, right, bottom, top);`
- **Modern OpenGL**: Matrix multiplication with an orthographic projection matrix in the vertex shader:
  $$\text{NDC}_x = \frac{2(x - \text{left})}{\text{right} - \text{left}} - 1, \quad \text{NDC}_y = \frac{2(y - \text{bottom})}{\text{top} - \text{bottom}} - 1$$

---

## 3. Geometric Primitives & Assembly Topologies

OpenGL renders all 2D and 3D shapes from fundamental primitive types:

| Primitive Mode | Vertex Requirement | Visual Description | Best Used For |
|---|---|---|---|
| `GL_POINTS` | 1 vertex per point | Independent dots rasterized with `glPointSize` | Stars, particles, point grids |
| `GL_LINES` | 2 vertices per segment | Disconnected line segments $(V_0 \to V_1)$, $(V_2 \to V_3)$ | Grid lines, disconnected axes |
| `GL_LINE_STRIP` | $N$ vertices | Connected continuous path $(V_0 \to V_1 \to V_2 \dots)$ | Line graphs, open curves |
| `GL_LINE_LOOP` | $N$ vertices | Connected path automatically closing back to $V_0$ | Hollow polygons, shape outlines |
| `GL_TRIANGLES` | 3 vertices per triangle | Independent triangles $(V_0, V_1, V_2), (V_3, V_4, V_5)$ | Universal mesh building block |
| `GL_TRIANGLE_STRIP` | $N$ vertices | Connected triangles sharing edges: $(V_0, V_1, V_2), (V_1, V_2, V_3)$ | Terrains, ribbons, quads |
| `GL_TRIANGLE_FAN` | $N$ vertices | Central vertex $V_0$ connected to fan $(V_0, V_1, V_2), (V_0, V_2, V_3)$ | Circles, disks, convex fans |
| `GL_QUADS` *(Legacy)* | 4 vertices per quad | 4-sided filled polygons | Buildings, walls, 2D planes |
| `GL_POLYGON` *(Legacy)* | $N$ vertices | Single convex filled polygon | Complex flat shapes |

> [!NOTE]
> In Modern Core Profile OpenGL (3.3+), `GL_QUADS` and `GL_POLYGON` are deprecated and removed. All 2D and 3D quadrilaterals and polygons are constructed using **two triangles** via `GL_TRIANGLES` or `GL_TRIANGLE_STRIP`.

---

## 4. Modern OpenGL Data Architecture: VAO, VBO, and EBO

### 1. Vertex Buffer Object (VBO)
A VBO is a high-speed GPU memory buffer that stores raw vertex data (positions, colors, normals, UVs). Storing data directly in GPU VRAM avoids per-frame CPU-to-GPU data transmission bottlenecks.

### 2. Vertex Array Object (VAO)
A VAO stores the configuration of vertex attribute pointers and which VBOs to pull data from. Once bound, all subsequent vertex attribute pointer configurations and EBO bindings are stored inside this VAO.

### 3. Element Buffer Object (EBO / IBO)
An EBO stores vertex indices to prevent duplicate vertex definitions. For example, a rectangle has 4 unique corners but requires 6 vertices (2 triangles) to render; an EBO references the 4 vertices by index `[0, 1, 2, 2, 3, 0]`.

```
                    ┌───────────────────────────────┐
                    │     Vertex Array Object (VAO) │
                    └───────┬───────────────┬───────┘
                            │               │
                            ▼               ▼
                ┌──────────────────┐ ┌──────────────────┐
                │ Attribute 0: Pos │ │ Attribute 1: Col │
                └───────────┬──────┘ └──────────┬───────┘
                            │                   │
                            ▼                   ▼
    ┌─────────────────────────────────────────────────────────────┐
    │                 Vertex Buffer Object (VBO)                  │
    │ [ X0, Y0, Z0,  R0, G0, B0 ] [ X1, Y1, Z1,  R1, G1, B1 ] ... │
    └─────────────────────────────────────────────────────────────┘
                                    ▲
                                    │ (Indexed by)
    ┌───────────────────────────────┴─────────────────────────────┐
    │                 Element Buffer Object (EBO)                 │
    │                      [ 0, 1, 2,  2, 3, 0 ]                  │
    └─────────────────────────────────────────────────────────────┘
```

### Memory Layout & Stride/Offset Calculation
When vertex positions and colors are **interleaved** in a single buffer:

```cpp
float vertices[] = {
    // Positions (X, Y, Z)     // Colors (R, G, B)
     0.5f,  0.5f, 0.0f,        1.0f, 0.0f, 0.0f,   // Vertex 0 (Top-Right)
     0.5f, -0.5f, 0.0f,        0.0f, 1.0f, 0.0f,   // Vertex 1 (Bottom-Right)
    -0.5f, -0.5f, 0.0f,        0.0f, 0.0f, 1.0f,   // Vertex 2 (Bottom-Left)
    -0.5f,  0.5f, 0.0f,        1.0f, 1.0f, 0.0f    // Vertex 3 (Top-Left)
};
```

- **Stride**: Byte distance to the next vertex = `6 * sizeof(float)` ($24\text{ bytes}$).
- **Position Offset**: Starts at index `0` $\rightarrow$ `(void*)0`.
- **Color Offset**: Starts after 3 floats $\rightarrow$ `(void*)(3 * sizeof(float))`.

```cpp
// Position attribute (location = 0)
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

// Color attribute (location = 1)
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
glEnableVertexAttribArray(1);
```

---

## 5. GLSL Shading Language for Static Graphics

### A. Static Shader Program with Per-Vertex Color Interpolation

#### Vertex Shader (`vertex.glsl`)
```glsl
#version 330 core
layout (location = 0) in vec3 aPos;   // Vertex position attribute
layout (location = 1) in vec3 aColor; // Vertex color attribute

out vec3 ourColor; // Output variable passed to fragment shader

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    ourColor = aColor; // Forward color to rasterizer for interpolation
}
```

#### Fragment Shader (`fragment.glsl`)
```glsl
#version 330 core
in vec3 ourColor;      // Interpolated color received from vertex shader
out vec4 FragColor;    // Final rasterized output pixel color

void main()
{
    FragColor = vec4(ourColor, 1.0f);
}
```

---

### B. Static Color Uniform Shader (Single Flat Tint)
If the color is constant across an entire object, use a **Uniform**:

#### Fragment Shader with Uniform
```glsl
#version 330 core
out vec4 FragColor;
uniform vec4 u_StaticColor; // Uniform variable set from C++

void main()
{
    FragColor = u_StaticColor;
}
```

#### C++ Uniform Assignment
```cpp
// Query uniform location
int colorLocation = glGetUniformLocation(shaderProgram, "u_StaticColor");

// Activate shader program before sending uniform values
glUseProgram(shaderProgram);

// Set RGBA color (e.g., Forest Green: 0.13, 0.55, 0.13, 1.0)
glUniform4f(colorLocation, 0.13f, 0.55f, 0.13f, 1.0f);
```

---

## 6. Foundational Programming Concepts Written as C++ Equivalents

Understanding what OpenGL and GLSL do under the hood helps you debug shader and geometry pipeline issues. Below are foundational math and graphics pipeline primitives written in pure standard C++.

### A. Vector Mathematics (`vec2`, `vec3`, `vec4`)
GLSL provides built-in vector types with operator overloading and component access. Here is their C++ equivalent:

```cpp
#include <cmath>
#include <iostream>

struct Vec2 {
    float x, y;
    Vec2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
    Vec2 operator+(const Vec2& o) const { return Vec2(x + o.x, y + o.y); }
    Vec2 operator-(const Vec2& o) const { return Vec2(x - o.x, y - o.y); }
    Vec2 operator*(float s) const { return Vec2(x * s, y * s); }
    float dot(const Vec2& o) const { return x * o.x + y * o.y; }
    float length() const { return std::sqrt(dot(*this)); }
    Vec2 normalize() const {
        float len = length();
        return len > 0.00001f ? (*this) * (1.0f / len) : Vec2(0, 0);
    }
};

struct Vec3 {
    float x, y, z;
    Vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
    Vec3 operator+(const Vec3& o) const { return Vec3(x + o.x, y + o.y, z + o.z); }
    Vec3 operator-(const Vec3& o) const { return Vec3(x - o.x, y - o.y, z - o.z); }
    Vec3 operator*(float s) const { return Vec3(x * s, y * s, z * s); }
    float dot(const Vec3& o) const { return x * o.x + y * o.y + z * o.z; }
    Vec3 cross(const Vec3& o) const {
        return Vec3(
            y * o.z - z * o.y,
            z * o.x - x * o.z,
            x * o.y - y * o.x
        );
    }
    float length() const { return std::sqrt(dot(*this)); }
    Vec3 normalize() const {
        float len = length();
        return len > 0.00001f ? (*this) * (1.0f / len) : Vec3(0, 0, 0);
    }
};

struct Vec4 {
    float x, y, z, w;
    Vec4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f)
        : x(x), y(y), z(z), w(w) {}
    Vec4 operator*(float s) const { return Vec4(x * s, y * s, z * s, w * s); }
};
```

---

### B. GLSL Built-in Math Functions in C++
GLSL features such as `mix`, `clamp`, `step`, and `smoothstep` are standard building blocks of shaders.

```cpp
namespace GLSL {
    // clamp: locks value x into the range [minVal, maxVal]
    inline float clamp(float x, float minVal, float maxVal) {
        if (x < minVal) return minVal;
        if (x > maxVal) return maxVal;
        return x;
    }

    // mix: linear interpolation between x and y based on a (a in [0, 1])
    inline float mix(float x, float y, float a) {
        return x * (1.0f - a) + y * a;
    }

    inline Vec3 mix(const Vec3& x, const Vec3& y, float a) {
        return x * (1.0f - a) + y * a;
    }

    // step: returns 0.0 if x < edge, else 1.0
    inline float step(float edge, float x) {
        return x < edge ? 0.0f : 1.0f;
    }

    // smoothstep: Hermite interpolation between edge0 and edge1
    inline float smoothstep(float edge0, float edge1, float x) {
        float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }
}
```

---

### C. 4x4 Transformation Matrix in C++ (`mat4`)
In modern OpenGL, transformations are represented by $4 \times 4$ column-major matrices:

```cpp
struct Mat4 {
    // Column-major order: elements[col * 4 + row]
    float m[16] = {
        1, 0, 0, 0,  // Col 0
        0, 1, 0, 0,  // Col 1
        0, 0, 1, 0,  // Col 2
        0, 0, 0, 1   // Col 3
    };

    static Mat4 identity() {
        return Mat4();
    }

    static Mat4 translation(float tx, float ty, float tz) {
        Mat4 res = identity();
        res.m[12] = tx; // Column 3, Row 0
        res.m[13] = ty; // Column 3, Row 1
        res.m[14] = tz; // Column 3, Row 2
        return res;
    }

    static Mat4 scale(float sx, float sy, float sz) {
        Mat4 res;
        res.m[0] = sx;
        res.m[5] = sy;
        res.m[10] = sz;
        res.m[15] = 1.0f;
        return res;
    }

    static Mat4 rotateZ(float radians) {
        Mat4 res = identity();
        float c = std::cos(radians);
        float s = std::sin(radians);
        res.m[0] =  c; res.m[4] = -s;
        res.m[1] =  s; res.m[5] =  c;
        return res;
    }

    // Matrix Multiplication (A * B)
    Mat4 operator*(const Mat4& b) const {
        Mat4 res;
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    sum += m[k * 4 + row] * b.m[col * 4 + k];
                }
                res.m[col * 4 + row] = sum;
            }
        }
        return res;
    }

    // Transform 4D Vector (M * v)
    Vec4 multiply(const Vec4& v) const {
        return Vec4(
            m[0]*v.x + m[4]*v.y + m[8]*v.z  + m[12]*v.w,
            m[1]*v.x + m[5]*v.y + m[9]*v.z  + m[13]*v.w,
            m[2]*v.x + m[6]*v.y + m[10]*v.z + m[14]*v.w,
            m[3]*v.x + m[7]*v.y + m[11]*v.z + m[15]*v.w
        );
    }
};
```

---

### D. Software Emulation of Rasterizer Color Interpolation
When OpenGL renders a triangle with different colors at each vertex, the rasterizer computes **barycentric coordinates** $(\lambda_1, \lambda_2, \lambda_3)$ for every fragment inside the triangle:

```cpp
// Barycentric interpolation equivalent to OpenGL rasterization
struct Vertex {
    Vec2 position;
    Vec3 color;
};

Vec3 interpolateFragmentColor(const Vertex& v1, const Vertex& v2, const Vertex& v3, const Vec2& p) {
    // Compute barycentric coordinates (areas of sub-triangles)
    float denom = (v2.position.y - v3.position.y) * (v1.position.x - v3.position.x) +
                  (v3.position.x - v2.position.x) * (v1.position.y - v3.position.y);

    float w1 = ((v2.position.y - v3.position.y) * (p.x - v3.position.x) +
                (v3.position.x - v2.position.x) * (p.y - v3.position.y)) / denom;

    float w2 = ((v3.position.y - v1.position.y) * (p.x - v3.position.x) +
                (v1.position.x - v3.position.x) * (p.y - v3.position.y)) / denom;

    float w3 = 1.0f - w1 - w2;

    // Linearly interpolate RGB components across the triangle surface
    return v1.color * w1 + v2.color * w2 + v3.color * w3;
}
```

---

## 7. Practical Code Snippets & Case Studies

### Case Study 1: Modern OpenGL Static Scene with EBO (GLFW + GLAD)

Below is a complete, standalone template for rendering a static colored rectangle with indexed vertices using Modern OpenGL Core Profile:

```cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Vertex Shader
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

void main() {
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
}
)";

// Fragment Shader
const char* fragmentShaderSource = R"(
#version 330 core
in vec3 ourColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(ourColor, 1.0);
}
)";

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Modern OpenGL - Static Scene", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 1. Compile Shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 2. Vertex Data with Interleaved Positions and Colors
    float vertices[] = {
        // Positions          // Colors (RGB)
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f, // Top-Right (Red)
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f, // Bottom-Right (Green)
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f, // Bottom-Left (Blue)
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f  // Top-Left (Yellow)
    };

    unsigned int indices[] = {
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };

    // 3. Create VAO, VBO, and EBO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position Attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color Attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 4. Render Loop
    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Clear Color Buffer
        glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw Geometry
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
```

---

### Case Study 2: Legacy OpenGL Static Scene (House with Sun, Roof & Walls)

Below is a representative legacy OpenGL (Code::Blocks / FreeGLUT) implementation demonstrating direct primitives:

```cpp
#include <GL/glut.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void drawCircle(float cx, float cy, float r, int segments = 50) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * M_PI * float(i) / float(segments);
        glVertex2f(cx + r * cosf(theta), cy + r * sinf(theta));
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // 1. Background Sky (Quad)
    glBegin(GL_QUADS);
        glColor3f(0.53f, 0.81f, 0.98f); // Light Sky Blue
        glVertex2f(-1.0f, -0.2f);
        glVertex2f( 1.0f, -0.2f);
        glVertex2f( 1.0f,  1.0f);
        glVertex2f(-1.0f,  1.0f);
    glEnd();

    // 2. Green Meadow (Quad)
    glBegin(GL_QUADS);
        glColor3f(0.20f, 0.80f, 0.20f); // Lime Green
        glVertex2f(-1.0f, -1.0f);
        glVertex2f( 1.0f, -1.0f);
        glVertex2f( 1.0f, -0.2f);
        glVertex2f(-1.0f, -0.2f);
    glEnd();

    // 3. Sun (Smooth Circle)
    glColor3f(1.0f, 0.84f, 0.0f); // Gold
    drawCircle(-0.7f, 0.7f, 0.15f);

    // 4. House Body (Quad)
    glBegin(GL_QUADS);
        glColor3f(0.94f, 0.90f, 0.55f); // Tan / Light Sand
        glVertex2f(-0.5f, -0.6f);
        glVertex2f( 0.2f, -0.6f);
        glVertex2f( 0.2f,  0.0f);
        glVertex2f(-0.5f,  0.0f);
    glEnd();

    // 5. House Roof (Triangle)
    glBegin(GL_TRIANGLES);
        glColor3f(0.86f, 0.08f, 0.24f); // Crimson Red
        glVertex2f(-0.55f,  0.0f);
        glVertex2f( 0.25f,  0.0f);
        glVertex2f(-0.15f,  0.4f);
    glEnd();

    // 6. Door (Quad)
    glBegin(GL_QUADS);
        glColor3f(0.55f, 0.27f, 0.07f); // Saddle Brown
        glVertex2f(-0.25f, -0.6f);
        glVertex2f(-0.05f, -0.6f);
        glVertex2f(-0.05f, -0.2f);
        glVertex2f(-0.25f, -0.2f);
    glEnd();

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Legacy OpenGL - Static House Scene");
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
```

---

## 8. Modern vs. Legacy OpenGL Comparison Cheat Sheet

| Feature | Legacy OpenGL (1.x - 2.x) | Modern OpenGL (3.3+ Core) |
|---|---|---|
| **Pipeline Architecture** | Fixed-Function Pipeline | Programmable Pipeline (Vertex & Fragment Shaders) |
| **Geometry Specification** | `glBegin()` / `glEnd()` / `glVertex2f()` | VBOs + VAOs + EBOs in GPU Memory |
| **Coloring** | `glColor3f(r, g, b)` state machine | Vertex Attributes, Fragment Shaders & Uniforms |
| **Coordinate Transformation** | Matrix stack (`glPushMatrix`, `glRotatef`) | Uniform matrices (`mat4`) computed in C++ and sent to GLSL |
| **Performance** | Slow (CPU-to-GPU data transmission every frame) | Ultra-fast (Data lives persistently on GPU VRAM) |
| **Code Structure** | Imperative, procedural immediate calls | Declarative buffer setup, shader programs, state binding |
