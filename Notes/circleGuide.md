# Circle Drawing Methods in OpenGL (CodeBlocks GLUT)

In OpenGL 2D Computer Graphics, there is no native `glDrawCircle()` function. Because OpenGL renders shapes using primitives (`GL_POINTS`, `GL_LINES`, `GL_TRIANGLES`, `GL_QUADS`, `GL_POLYGON`), circles are rendered by approximating a circular curve.

This guide covers the primary approaches used in Computer Graphics Lab projects:

---

## 1. Approach 1: Direct Manual `glVertex2f` Coordinates

### Concept
Similar to drawing triangles or quadrilaterals (as in `cgm lab-3.1`), you can specify fixed coordinate points around the circumference manually inside `glBegin(GL_POLYGON)` or `glBegin(GL_LINE_LOOP)`.

### Code Example
```cpp
#include <GL/glut.h>

void drawDirectManualCircle() {
    // 8-sided polygon approximation (Octagon)
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.2f, 0.2f); // Red
        glVertex2f( 0.00f,  0.30f); // Top
        glVertex2f( 0.21f,  0.21f); // Top-Right
        glVertex2f( 0.30f,  0.00f); // Right
        glVertex2f( 0.21f, -0.21f); // Bottom-Right
        glVertex2f( 0.00f, -0.30f); // Bottom
        glVertex2f(-0.21f, -0.21f); // Bottom-Left
        glVertex2f(-0.30f,  0.00f); // Left
        glVertex2f(-0.21f,  0.21f); // Top-Left
    glEnd();
}
```

### When to Use
- **Simple shapes** where high precision is not required.
- **Beginner assignments** introducing `GL_POLYGON` without math helper loops.

---

## 2. Approach 2: Trigonometric & Loop-Based Approximation

### Concept
Instead of writing 36 or 100 `glVertex2f` statements by hand, a `for` loop calculates points around the radius using trigonometric functions (`cos` and `sin`).

### A. Simple Degree-Based Loop (`0` to `360` degrees)
```cpp
#include <cmath>
#include <GL/glut.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void drawSimpleDegreeCircle(float cx, float cy, float radius) {
    glBegin(GL_POLYGON);
        glColor3f(0.2f, 0.6f, 1.0f); // Blue
        for (float angle = 0.0f; angle < 360.0f; angle += 5.0f) {
            float rad = angle * (M_PI / 180.0f); // Convert degree to radian
            float x = cx + radius * cos(rad);
            float y = cy + radius * sin(rad);
            glVertex2f(x, y);
        }
    glEnd();
}
```

### B. Segment-Based Radian Loop (`GL_TRIANGLE_FAN` / `GL_LINE_LOOP`)
```cpp
void drawSmoothCircle(float cx, float cy, float radius, int segments = 100) {
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy); // Center point
        for (int i = 0; i <= segments; ++i) {
            float theta = 2.0f * M_PI * float(i) / float(segments);
            float x = cx + radius * cosf(theta);
            float y = cy + radius * sinf(theta);
            glVertex2f(x, y);
        }
    glEnd();
}
```

### When to Use
- **Smooth circles** with scalable radii and positions.
- **Dynamic shapes** like filled circles (`GL_TRIANGLE_FAN`), outlines (`GL_LINE_LOOP`), or dotted circles (`GL_POINTS`).

---

## 3. Approach 3: Midpoint / Bresenham's Circle Algorithm

### Concept
Calculates pixel points around a circle using 8-way symmetry and integer additions, avoiding floating-point trigonometry.

```cpp
void drawMidpointCircle(float cx, float cy, float radius) {
    glPointSize(3.0f);
    glBegin(GL_POINTS);
        glColor3f(1.0f, 1.0f, 0.0f);
        int r = (int)(radius * 500);
        int x = 0, y = r, p = 1 - r;

        auto plot8Points = [&](int x, int y) {
            float s = 1.0f / 500.0f;
            glVertex2f(cx + x*s, cy + y*s); glVertex2f(cx - x*s, cy + y*s);
            glVertex2f(cx + x*s, cy - y*s); glVertex2f(cx - x*s, cy - y*s);
            glVertex2f(cx + y*s, cy + x*s); glVertex2f(cx - y*s, cy + x*s);
            glVertex2f(cx + y*s, cy - x*s); glVertex2f(cx - y*s, cy - x*s);
        };

        plot8Points(x, y);
        while (x < y) {
            x++;
            if (p < 0) p += 2 * x + 1;
            else { y--; p += 2 * (x - y) + 1; }
            plot8Points(x, y);
        }
    glEnd();
}
```

---

## Summary Comparison

| Approach | Syntax | Smoothness | Best Use Case |
|---|---|---|---|
| **Direct Manual Vertices** | Fixed `glVertex2f(...)` | Low (Polygon) | Quick static shapes without math loops |
| **Degree Loop (0-360)** | Simple `for (angle=0..360)` | High | Easy to write & remember |
| **Trig Radian Loop** | `cos(theta)`, `sin(theta)` | Very High | Scalable, reusable helper functions |
| **Midpoint Algorithm** | `GL_POINTS` with 8-way symmetry | Pixel Level | Theoretical Computer Graphics algorithms |
