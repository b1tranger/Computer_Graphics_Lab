# Drawing Colored Borders & Hollow Shapes in OpenGL (CodeBlocks GLUT)

In OpenGL 2D Computer Graphics, drawing **hollow shapes** (shapes with colored borders and no solid fill) is a fundamental technique. By default, primitive modes like `GL_POLYGON`, `GL_QUADS`, and `GL_TRIANGLES` create filled, solid shapes. To draw outlines or borders, OpenGL provides line-based primitives and wireframe polygon modes.

---

## 1. Overview: Primitive Modes for Borders

| Primitive / Command | Behavior | Best Used For |
|---|---|---|
| **`GL_LINE_LOOP`** | Connects vertices sequentially and automatically connects the last vertex back to the first. | Closed hollow shapes (rectangles, triangles, circles, polygons) |
| **`GL_LINE_STRIP`** | Connects vertices sequentially in a continuous open line, without closing the end. | Open paths, curves, arcs, polylines |
| **`GL_LINES`** | Draws independent line segments between pairs of vertices $(V_0 \rightarrow V_1)$, $(V_2 \rightarrow V_3)$. | Grid lines, tick marks, disconnected borders |
| **`glPolygonMode`** | Alters rendering mode of standard polygons to wireframe/outline without changing primitive code. | Quick outline toggling for complex polygon objects |

---

## 2. Line Styling: Thickness, Color, and Patterns

Before issuing vertex coordinates, you can customize the appearance of borders using line control functions.

### A. Line Thickness (`glLineWidth`)
Sets the width of rasterized lines in pixels. Default is `1.0f`.

```cpp
glLineWidth(3.0f); // Set thick border (3 pixels wide)
// ... draw lines or LINE_LOOP ...
glLineWidth(1.0f); // Reset back to standard width
```

> **Note**: Always reset `glLineWidth(1.0f)` after drawing to avoid affecting subsequent line drawing calls in your scene.

---

### B. Border Color (`glColor3f`)
Sets the border color. Color can be applied uniformly to the entire border or changed per-vertex to create multi-colored / gradient outlines.

```cpp
// Uniform Color Border
glColor3f(1.0f, 0.0f, 0.0f); // Red border
glBegin(GL_LINE_LOOP);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f( 0.5f, -0.5f);
    glVertex2f( 0.5f,  0.5f);
    glVertex2f(-0.5f,  0.5f);
glEnd();
```

---

### C. Dashed & Dotted Borders (`glLineStipple`)
To draw dashed or dotted outlines, enable line stippling in OpenGL.

```cpp
glEnable(GL_LINE_STIPPLE);

// Pattern examples (16-bit hex bitmask):
// 0x00FF = Dashed line (0000000011111111)
// 0x0F0F = Medium dash/dot (0000111100001111)
// 0x5555 = Dotted line (0101010101010101)

glLineStipple(1, 0x00FF); // Factor = 1 (multiplier), Pattern = 0x00FF

glLineWidth(2.0f);
glBegin(GL_LINE_LOOP);
    // Vertex coordinates...
glEnd();

glDisable(GL_LINE_STIPPLE); // Disable stippling when finished
```

---

## 3. Method 1: Using `GL_LINE_LOOP` (Recommended Approach)

`GL_LINE_LOOP` is the most common and straightforward method to draw closed hollow shapes.

### A. Hollow Rectangle / Square
```cpp
#include <GL/glut.h>

void drawHollowRectangle(float x, float y, float width, float height) {
    glLineWidth(2.5f);
    glColor3f(0.0f, 0.6f, 1.0f); // Sky Blue Border

    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);                   // Bottom-Left
        glVertex2f(x + width, y);           // Bottom-Right
        glVertex2f(x + width, y + height);  // Top-Right
        glVertex2f(x, y + height);          // Top-Left
    glEnd();

    glLineWidth(1.0f); // Reset width
}
```

---

### B. Hollow Triangle
```cpp
void drawHollowTriangle() {
    glLineWidth(3.0f);
    glColor3f(0.9f, 0.2f, 0.2f); // Crimson Red Border

    glBegin(GL_LINE_LOOP);
        glVertex2f( 0.0f,  0.5f); // Top Vertex
        glVertex2f(-0.5f, -0.4f); // Bottom-Left Vertex
        glVertex2f( 0.5f, -0.4f); // Bottom-Right Vertex
    glEnd();

    glLineWidth(1.0f);
}
```

---

### C. Hollow Circle / Arc Outline
Using a trigonometric loop with `GL_LINE_LOOP` creates a smooth hollow circular outline.

```cpp
#include <cmath>
#include <GL/glut.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Hollow Circle using GL_LINE_LOOP
void drawHollowCircle(float cx, float cy, float radius, int segments = 100) {
    glLineWidth(2.0f);
    glColor3f(0.1f, 0.8f, 0.3f); // Vibrant Green Border

    glBegin(GL_LINE_LOOP);
        for (int i = 0; i < segments; ++i) {
            float theta = 2.0f * M_PI * float(i) / float(segments);
            float x = cx + radius * cosf(theta);
            float y = cy + radius * sinf(theta);
            glVertex2f(x, y);
        }
    glEnd();

    glLineWidth(1.0f);
}

// Open Circular Arc using GL_LINE_STRIP
void drawCircularArc(float cx, float cy, float radius, float startAngleDeg, float endAngleDeg, int segments = 50) {
    glLineWidth(2.0f);
    glColor3f(1.0f, 0.5f, 0.0f); // Orange Arc Border

    glBegin(GL_LINE_STRIP); // GL_LINE_STRIP leaves the arc open at both ends
        for (int i = 0; i <= segments; ++i) {
            float angleDeg = startAngleDeg + (endAngleDeg - startAngleDeg) * ((float)i / segments);
            float rad = angleDeg * (M_PI / 180.0f);
            float x = cx + radius * cosf(rad);
            float y = cy + radius * sinf(rad);
            glVertex2f(x, y);
        }
    glEnd();

    glLineWidth(1.0f);
}
```

---

### D. Multi-Colored / Gradient Border
Assigning a different `glColor3f` to each vertex generates a smooth color gradient across the shape's border edges.

```cpp
void drawGradientBorderSquare() {
    glLineWidth(4.0f);

    glBegin(GL_LINE_LOOP);
        glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(-0.4f, -0.4f); // Red corner
        glColor3f(0.0f, 1.0f, 0.0f); glVertex2f( 0.4f, -0.4f); // Green corner
        glColor3f(0.0f, 0.0f, 1.0f); glVertex2f( 0.4f,  0.4f); // Blue corner
        glColor3f(1.0f, 1.0f, 0.0f); glVertex2f(-0.4f,  0.4f); // Yellow corner
    glEnd();

    glLineWidth(1.0f);
}
```

---

## 4. Method 2: Wireframe Mode via `glPolygonMode`

If you already have code that draws solid filled polygons (e.g., using `GL_POLYGON`, `GL_QUADS`, `GL_TRIANGLES`), you can render them as hollow borders without changing `glBegin()` primitive types by changing OpenGL's rasterization mode.

### Syntax
```cpp
glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Switch to outline/wireframe mode
```

### Code Example
```cpp
void drawWireframeHouse() {
    glLineWidth(2.0f);
    glColor3f(0.8f, 0.8f, 0.8f); // White/Gray wireframe lines

    // Enable wireframe mode for all polygons
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // House Base (drawn using standard polygon)
    glBegin(GL_QUADS);
        glVertex2f(-0.4f, -0.5f);
        glVertex2f( 0.4f, -0.5f);
        glVertex2f( 0.4f,  0.0f);
        glVertex2f(-0.4f,  0.0f);
    glEnd();

    // House Roof (drawn using standard triangle)
    glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, 0.0f);
        glVertex2f( 0.5f, 0.0f);
        glVertex2f( 0.0f, 0.4f);
    glEnd();

    // IMPORTANT: Revert back to solid fill mode for future drawings
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(1.0f);
}
```

---

## 5. Method 3: Combining Solid Fill with Outer Border

In many lab graphics assignments, a shape must have **both** a solid inner color AND a thick outer border color of a different shade (e.g., a Yellow star with a Dark Brown border).

### Two-Pass Rendering Technique

1. **Pass 1 (Fill)**: Draw filled primitive (`GL_POLYGON`) with the fill color.
2. **Pass 2 (Border)**: Draw identical vertices using `GL_LINE_LOOP` with the border color and desired line width.

```cpp
void drawBorderedShape() {
    // PASS 1: Draw Solid Fill (Inside)
    glColor3f(1.0f, 0.9f, 0.2f); // Gold/Yellow Fill Color
    glBegin(GL_POLYGON);
        glVertex2f(-0.3f, -0.3f);
        glVertex2f( 0.3f, -0.3f);
        glVertex2f( 0.4f,  0.2f);
        glVertex2f( 0.0f,  0.5f);
        glVertex2f(-0.4f,  0.2f);
    glEnd();

    // PASS 2: Draw Outer Border (Outline)
    glLineWidth(3.5f);
    glColor3f(0.4f, 0.1f, 0.0f); // Dark Brown Border Color
    glBegin(GL_LINE_LOOP);
        glVertex2f(-0.3f, -0.3f);
        glVertex2f( 0.3f, -0.3f);
        glVertex2f( 0.4f,  0.2f);
        glVertex2f( 0.0f,  0.5f);
        glVertex2f(-0.4f,  0.2f);
    glEnd();

    glLineWidth(1.0f); // Reset line width
}
```

---

## 6. Summary & Quick Comparison

| Technique | Implementation | Pros | Cons |
|---|---|---|---|
| **`GL_LINE_LOOP`** | Change primitive inside `glBegin(GL_LINE_LOOP)` | Cleanest, explicit, automatic closing segment | Must change primitive call |
| **`GL_LINE_STRIP`** | `glBegin(GL_LINE_STRIP)` | Ideal for open borders and arcs | Does not auto-connect last & first vertex |
| **`glPolygonMode`** | `glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)` | Keeps existing `GL_POLYGON` code unmodified | Global state; requires switching back with `GL_FILL` |
| **Two-Pass (Fill + Border)** | Render `GL_POLYGON` then `GL_LINE_LOOP` | Produces professional filled shapes with distinct outlines | Requires duplicating vertex list or using helper functions |
