# Comprehensive Guide to Dynamic Graphics & Animations in OpenGL (Modern & Legacy)

In computer graphics, **dynamic graphics** and **animations** refer to visual scenes where objects change position, orientation, scale, shape, or color over time. Rather than generating a single static image, the application continuously generates and displays successive frames to create the illusion of smooth motion (persistence of vision).

This guide covers the theoretical principles of real-time animation, frame-rate independence, delta time ($\Delta t$), double buffering, modern GPU shader-based animations, matrix transformations, interactive input control, and foundational physics/math implementations in pure C++.

---

## 1. Core Principles of Real-Time Animation

### The Render Loop Architecture
Every dynamic OpenGL application operates on a continuous loop consisting of four sequential phases:

```
┌─────────────────────────────────────────────────────────────┐
│                      THE RENDER LOOP                        │
├─────────────────────────────────────────────────────────────┤
│ 1. Process Input        --> Keyboard, Mouse, Window resize  │
│ 2. Update Simulation    --> Calculate Delta Time, Physics,  │
│                             Positions, Angles, Colors       │
│ 3. Render / Draw        --> Clear buffers, bind shaders,    │
│                             upload uniforms, issue draw call│
│ 4. Swap Buffers & Poll  --> Double buffering presentation,  │
│                             OS message dispatch             │
└─────────────────────────────────────────────────────────────┘
```

---

### Double Buffering & Screen Tearing
When rendering animated scenes, drawing directly to the screen produces **screen tearing** (where the top half shows the new frame while the bottom half still shows the old frame).

OpenGL prevents tearing using **Double Buffering**:
* **Front Buffer**: Contains the completed image currently displayed by the monitor.
* **Back Buffer**: An off-screen buffer where OpenGL draws the next frame.
* **Buffer Swapping**: Once rendering finishes, the front and back buffers are swapped instantly:
  - **Modern GLFW**: `glfwSwapBuffers(window);`
  - **Legacy GLUT**: `glutSwapBuffers();` (enabled with `GLUT_DOUBLE`)

```
               [ OpenGL Draw Calls ]
                         │
                         ▼
┌──────────────────┐           ┌──────────────────┐
│   Back Buffer    │ ──SWAP──> │   Front Buffer   │ ──> [ Monitor Display ]
│ (Drawing next)   │           │ (Currently seen) │
└──────────────────┘           └──────────────────┘
```

---

### Frame-Rate Independence & Delta Time ($\Delta t$)

> [!WARNING]
> Never update animated positions by adding a fixed constant per frame (e.g. `x += 0.01f;`).
> On a 60 Hz monitor, the object moves at 60 steps/sec. On a 144 Hz monitor, it moves more than twice as fast!

To make motion consistent across all hardware, scale all speeds and velocities by **Delta Time ($\Delta t$)** — the exact time elapsed between the current and previous frame.

$$\text{Position}_{\text{new}} = \text{Position}_{\text{old}} + (\text{Velocity} \times \Delta t)$$

#### Delta Time Calculation in Modern OpenGL (GLFW)
```cpp
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

while (!glfwWindowShouldClose(window)) {
    // 1. Calculate per-frame delta time
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // 2. Update position using velocity (units per second)
    float speed = 1.5f; // Move 1.5 NDC units per second
    posX += speed * deltaTime;

    // ... render frame ...
}
```

---

## 2. Animation Mechanisms in OpenGL

There are four primary mechanisms used to produce dynamic graphics:

| Mechanism | Pipeline Location | Description | Performance | Best Use Case |
|---|---|---|---|---|
| **Shader Uniforms (Math)** | GPU (Fragment/Vertex) | Time scalar `u_Time` passed to GLSL; math equations animate color/geometry | Ultra High | Color pulsing, water ripples, waving flags |
| **Matrix Uniforms** | CPU $\to$ GPU | C++ computes Model matrix ($T \times R \times S$) and uploads via `glUniformMatrix4fv` | High | Moving characters, rotating wheels, orbiting planets |
| **Dynamic VBO Update** | CPU $\to$ GPU VRAM | C++ updates raw vertex arrays with `glBufferSubData` | Medium | Particle engines, morphing meshes, terrain deformation |
| **Timer / Idle Callbacks** | CPU (Legacy GLUT) | `glutTimerFunc` or `glutIdleFunc` triggers `glutPostRedisplay()` | Low | Educational assignments, basic 2D animations |

---

## 3. Core Motion Types & Mathematical Formulations

### A. Linear Motion & Kinematics
Movement along a straight line governed by velocity and constant acceleration (such as gravity):

$$\vec{v} = \vec{v}_0 + \vec{a} \cdot \Delta t, \quad \vec{p} = \vec{p}_0 + \vec{v} \cdot \Delta t$$

```cpp
struct KinematicBody2D {
    float posX = 0.0f, posY = 0.5f;
    float velX = 0.8f, velY = 0.0f;
    float gravity = -9.8f;

    void update(float dt) {
        velY += gravity * dt; // Apply acceleration
        posX += velX * dt;    // Apply velocity
        posY += velY * dt;

        // Bounce off bottom floor (NDC Y = -0.9)
        if (posY < -0.9f) {
            posY = -0.9f;
            velY = -velY * 0.75f; // Invert and dampen (75% restitution)
        }
    }
};
```

---

### B. Rotational & Circular Orbital Motion
Rotating an object around a pivot $(c_x, c_y)$ at radius $R$ with angular speed $\omega$:

$$x = c_x + R \cos(\theta), \quad y = c_y + R \sin(\theta), \quad \text{where } \theta = \theta_0 + \omega \cdot t$$

```cpp
float orbitAngle = 0.0f;
float orbitSpeed = 2.0f; // Radians per second
float orbitRadius = 0.5f;

void updateOrbit(float dt, float& outX, float& outY) {
    orbitAngle += orbitSpeed * dt;
    outX = orbitRadius * std::cos(orbitAngle);
    outY = orbitRadius * std::sin(orbitAngle);
}
```

---

### C. Harmonic Oscillation (Sine & Cosine Waves)
Smooth back-and-forth movement, hovering, or pulsating scale:

$$y(t) = A \cdot \sin(2\pi f \cdot t + \phi)$$

- $A$ = Amplitude (maximum extent of movement)
- $f$ = Frequency (oscillations per second)
- $\phi$ = Phase offset

```cpp
// Hovering floating effect
float time = static_cast<float>(glfwGetTime());
float hoverOffset = 0.1f * std::sin(time * 3.0f); // Amplitude 0.1, Frequency 3.0 rad/s
float currentY = baseY + hoverOffset;

// Pulsating scale factor (breathes between 0.8x and 1.2x)
float scaleFactor = 1.0f + 0.2f * std::sin(time * 2.5f);
```

---

### D. Interpolation & Easing Curves
Transitioning between two values $A$ and $B$ smoothly over normalized time $t \in [0, 1]$:

```
Linear:         f(t) = t
Ease-In (Quad): f(t) = t^2
Ease-Out:       f(t) = 1 - (1 - t)^2
Smoothstep:     f(t) = 3t^2 - 2t^3
```

---

## 4. GLSL Shading Language Snippets for Dynamic Graphics

### A. Dynamic Color-Pulsing Fragment Shader
Pass elapsed time as a uniform to pulse RGB colors dynamically on the GPU:

```glsl
#version 330 core
out vec4 FragColor;

uniform float u_Time; // Elapsed time in seconds passed from C++

void main()
{
    // Generate pulsating color components oscillating smoothly between 0.1 and 1.0
    float red   = 0.55 + 0.45 * sin(u_Time * 2.0);
    float green = 0.55 + 0.45 * sin(u_Time * 2.0 + 2.094); // 120-degree phase shift
    float blue  = 0.55 + 0.45 * sin(u_Time * 2.0 + 4.188); // 240-degree phase shift

    FragColor = vec4(red, green, blue, 1.0);
}
```

---

### B. Dynamic Waving / Ripple Vertex Shader
Displaces vertex heights in real-time along a sine wave:

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;
uniform float u_Time;

void main()
{
    // Apply dynamic sine wave offset based on X position and elapsed time
    float waveAmplitude = 0.1;
    float waveFrequency = 5.0;
    float waveSpeed = 3.0;

    float newY = aPos.y + waveAmplitude * sin(aPos.x * waveFrequency + u_Time * waveSpeed);

    gl_Position = vec4(aPos.x, newY, aPos.z, 1.0);
    ourColor = aColor;
}
```

---

### C. Dynamic Transformation Matrix Vertex Shader
Transforms geometry using a $4 \times 4$ Model matrix computed on the CPU:

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 u_Model; // Combined Translation * Rotation * Scale matrix

void main()
{
    gl_Position = u_Model * vec4(aPos, 1.0);
}
```

#### Uploading the Matrix from C++:
```cpp
int modelLoc = glGetUniformLocation(shaderProgram, "u_Model");
glUseProgram(shaderProgram);
// Send 1 matrix of 16 floats, GL_FALSE because our C++ matrix is already column-major
glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMatrix.m);
```

---

## 5. Foundational Programming Concepts Written as C++ Equivalents

### A. High-Precision Game Loop & Delta Time Engine
A production-grade C++ game loop managing real-time delta time calculation:

```cpp
#include <chrono>
#include <thread>

class Clock {
private:
    std::chrono::high_resolution_clock::time_point lastTime;

public:
    Clock() : lastTime(std::chrono::high_resolution_clock::now()) {}

    float getDeltaTime() {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;
        lastTime = currentTime;
        return elapsed.count(); // Returns delta time in seconds (e.g., 0.0166s for 60fps)
    }
};
```

---

### B. 2D Transform & Kinematics Engine in Pure C++
Emulates the transformation hierarchy and physics updates:

```cpp
#include <cmath>

struct Vector2D {
    float x = 0.0f, y = 0.0f;
    Vector2D operator+(const Vector2D& o) const { return {x + o.x, y + o.y}; }
    Vector2D operator-(const Vector2D& o) const { return {x - o.x, y - o.y}; }
    Vector2D operator*(float s) const { return {x * s, y * s}; }
    Vector2D& operator+=(const Vector2D& o) { x += o.x; y += o.y; return *this; }
};

struct Transform2D {
    Vector2D position{0.0f, 0.0f};
    float rotation = 0.0f; // Radians
    Vector2D scale{1.0f, 1.0f};
};

class PhysicsObject2D {
public:
    Transform2D transform;
    Vector2D velocity{0.0f, 0.0f};
    Vector2D acceleration{0.0f, -9.8f}; // Constant downward gravity
    float angularVelocity = 1.5f;       // Radians per second
    float restitution = 0.8f;           // Bounciness coefficient
    float halfSize = 0.1f;              // Extent for boundary collision

    void update(float dt) {
        // 1. Semi-Implicit Euler Integration
        velocity += acceleration * dt;
        transform.position += velocity * dt;
        transform.rotation += angularVelocity * dt;

        // 2. Screen Boundary Collisions (NDC range: [-1.0, 1.0])
        // Floor Collision
        if (transform.position.y - halfSize < -1.0f) {
            transform.position.y = -1.0f + halfSize;
            velocity.y = -velocity.y * restitution; // Reverse & dampen
        }
        // Left & Right Wall Collisions
        if (transform.position.x - halfSize < -1.0f) {
            transform.position.x = -1.0f + halfSize;
            velocity.x = -velocity.x * restitution;
        } else if (transform.position.x + halfSize > 1.0f) {
            transform.position.x = 1.0f - halfSize;
            velocity.x = -velocity.x * restitution;
        }
    }
};
```

---

### C. Dynamic CPU Matrix Transformation Generator
Computes composite transformation matrices ($M = T \times R \times S$) to upload to GLSL:

```cpp
struct Matrix4x4 {
    float m[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};

    static Matrix4x4 createTransform(float tx, float ty, float angleRad, float sx, float sy) {
        Matrix4x4 res;
        float cosA = std::cos(angleRad);
        float sinA = std::sin(angleRad);

        // Combined Translation * Rotation * Scale (Column-Major for OpenGL)
        res.m[0] =  cosA * sx;
        res.m[1] =  sinA * sx;
        res.m[4] = -sinA * sy;
        res.m[5] =  cosA * sy;
        res.m[12] = tx;
        res.m[13] = ty;
        return res;
    }
};
```

---

### D. Easing & Interpolation Functions in C++
Exact equivalents to GLSL and animation math:

```cpp
namespace AnimationMath {
    inline float lerp(float start, float end, float t) {
        return start + t * (end - start);
    }

    inline float easeInQuad(float t) {
        return t * t;
    }

    inline float easeOutQuad(float t) {
        return t * (2.0f - t);
    }

    inline float easeInOutCubic(float t) {
        return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
    }

    inline float smoothstep(float edge0, float edge1, float x) {
        float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }
}
```

---

## 6. Complete Code Snippets & Case Studies

### Case Study 1: Modern OpenGL Animated Scene (GLFW + GLAD + GLSL)

The following program renders a triangle that simultaneously **rotates**, **orbits**, and **pulses colors** dynamically using delta time and GLSL uniforms:

```cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

// Vertex Shader with Model Matrix Uniform
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;
uniform mat4 u_Model;

void main() {
    gl_Position = u_Model * vec4(aPos, 1.0);
    ourColor = aColor;
}
)";

// Fragment Shader with Time-Based Color Tint Uniform
const char* fragmentShaderSource = R"(
#version 330 core
in vec3 ourColor;
out vec4 FragColor;

uniform float u_Time;

void main() {
    // Pulse brightness with a sine wave
    float pulse = 0.7 + 0.3 * sin(u_Time * 4.0);
    FragColor = vec4(ourColor * pulse, 1.0);
}
)";

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Modern OpenGL - Dynamic Animation", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    // 1. Compile Shaders
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, NULL);
    glCompileShader(vs);

    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, NULL);
    glCompileShader(fs);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);

    // 2. Vertex Data
    float vertices[] = {
        // Positions         // Colors
         0.0f,  0.25f, 0.0f,  1.0f, 0.2f, 0.2f, // Top
        -0.2f, -0.20f, 0.0f,  0.2f, 1.0f, 0.2f, // Bottom-Left
         0.2f, -0.20f, 0.0f,  0.2f, 0.2f, 1.0f  // Bottom-Right
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Uniform locations
    int modelLoc = glGetUniformLocation(program, "u_Model");
    int timeLoc  = glGetUniformLocation(program, "u_Time");

    // Animation state variables
    float rotationAngle = 0.0f;
    float orbitAngle = 0.0f;
    float lastFrameTime = static_cast<float>(glfwGetTime());

    // 3. Render Loop
    while (!glfwWindowShouldClose(window)) {
        // Delta time
        float currentTime = static_cast<float>(glfwGetTime());
        float dt = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        // Input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Update Animation States
        rotationAngle += 3.0f * dt; // Self-rotation (3 rad/sec)
        orbitAngle += 1.5f * dt;    // Circular orbit path (1.5 rad/sec)

        float orbitRadius = 0.45f;
        float posX = orbitRadius * std::cos(orbitAngle);
        float posY = orbitRadius * std::sin(orbitAngle);

        // Construct 4x4 Model Matrix: Translation * Rotation
        float cosR = std::cos(rotationAngle);
        float sinR = std::sin(rotationAngle);
        float modelMatrix[16] = {
             cosR,  sinR, 0.0f, 0.0f,
            -sinR,  cosR, 0.0f, 0.0f,
             0.0f,  0.0f, 1.0f, 0.0f,
             posX,  posY, 0.0f, 1.0f
        };

        // Render
        glClearColor(0.08f, 0.09f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMatrix);
        glUniform1f(timeLoc, currentTime);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(program);
    glfwTerminate();
    return 0;
}
```

---

### Case Study 2: Interactive Keyboard-Controlled Object with Smooth Physics

In real-time games, continuous movement requires polling key states each frame and updating velocities smoothly:

```cpp
struct Player {
    float x = 0.0f, y = 0.0f;
    float vx = 0.0f, vy = 0.0f;
    float speed = 2.0f;     // Acceleration rate
    float friction = 0.88f; // Velocity damping factor
    float size = 0.1f;

    void processInput(GLFWwindow* window, float dt) {
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            vy += speed * dt;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            vy -= speed * dt;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            vx -= speed * dt;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            vx += speed * dt;
    }

    void update(float dt) {
        // Apply velocity
        x += vx;
        y += vy;

        // Apply friction damping
        vx *= friction;
        vy *= friction;

        // Clamp inside NDC viewport [-1.0, 1.0]
        if (x - size < -1.0f) { x = -1.0f + size; vx = 0; }
        if (x + size >  1.0f) { x =  1.0f - size; vx = 0; }
        if (y - size < -1.0f) { y = -1.0f + size; vy = 0; }
        if (y + size >  1.0f) { y =  1.0f - size; vy = 0; }
    }
};
```

---

### Case Study 3: Legacy OpenGL Dynamic Animation (FreeGLUT `glutTimerFunc`)

In legacy OpenGL, animations are driven using `glutTimerFunc` which triggers frame updates at a fixed millisecond interval:

```cpp
#include <GL/glut.h>
#include <cmath>

float ballX = 0.0f;
float ballY = 0.0f;
float speedX = 0.015f;
float speedY = 0.022f;
float ballRadius = 0.1f;
float rotationAngle = 0.0f;

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glPushMatrix();
        // 1. Move to ball's dynamic position
        glTranslatef(ballX, ballY, 0.0f);
        // 2. Rotate around its own center
        glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);

        // Draw animated colored square
        glBegin(GL_QUADS);
            glColor3f(1.0f, 0.2f, 0.2f); glVertex2f(-ballRadius, -ballRadius);
            glColor3f(0.2f, 1.0f, 0.2f); glVertex2f( ballRadius, -ballRadius);
            glColor3f(0.2f, 0.2f, 1.0f); glVertex2f( ballRadius,  ballRadius);
            glColor3f(1.0f, 1.0f, 0.2f); glVertex2f(-ballRadius,  ballRadius);
        glEnd();
    glPopMatrix();

    glutSwapBuffers(); // Swap front/back buffers in double-buffered mode
}

void timer(int value) {
    // Update position
    ballX += speedX;
    ballY += speedY;
    rotationAngle += 3.0f;

    // Bounce off NDC boundaries [-1.0, 1.0]
    if (ballX + ballRadius > 1.0f || ballX - ballRadius < -1.0f) {
        speedX = -speedX;
    }
    if (ballY + ballRadius > 1.0f || ballY - ballRadius < -1.0f) {
        speedY = -speedY;
    }

    // Request redraw
    glutPostRedisplay();

    // Re-register timer for next frame (approx 60 FPS = 16 milliseconds per frame)
    glutTimerFunc(16, timer, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // Enable double buffering
    glutInitWindowSize(800, 600);
    glutCreateWindow("Legacy OpenGL - Dynamic Bouncing Animation");

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0); // Start timer immediately
    glutMainLoop();
    return 0;
}
```

---

## 7. Performance & Troubleshooting Best Practices

### 1. Avoid Allocating Buffers Inside the Render Loop
* **Mistake**: Calling `glGenBuffers` or `glBufferData` inside `while (!glfwWindowShouldClose(window))` creates massive GPU memory leaks and drops frame rate to single digits.
* **Fix**: Allocate buffers (`glGenVertexArrays`, `glGenBuffers`, `glBufferData`) **once during setup**. Inside the render loop, only bind VAOs and issue draw calls (`glDrawArrays` / `glDrawElements`).

### 2. Enable V-Sync to Eliminate Tearing & GPU Overheating
* Call `glfwSwapInterval(1);` immediately after `glfwMakeContextCurrent(window);`. This locks rendering to the monitor's refresh rate (e.g. 60Hz or 144Hz), saving GPU energy and preventing visual artifacts.

### 3. CPU vs. GPU Animation Workload Balancing
* Use **GLSL Uniforms and Vertex Shaders** for rigid body transformations and geometric distortions. The GPU computes millions of vertex positions in parallel.
* Keep **game logic, collision detection, and AI** on the CPU, uploading final translation/rotation values via uniforms.
