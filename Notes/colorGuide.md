# OpenGL Color & Gradient Guide (`glColor3f`)

In OpenGL, colors are defined using the **RGB (Red, Green, Blue)** color model. The function `glColor3f(r, g, b)` sets the active color for all subsequent vertices.

---

## 1. Understanding `glColor3f(r, g, b)` Syntax

- Each parameter (`r`, `g`, `b`) accepts a floating-point value from **`0.0f` (0%) to `1.0f` (100%)**.
- **RGB Conversion Formula**: If you have standard RGB values (0 to 255), divide each component by `255.0f`:
  $$\text{OpenGL Color Component} = \frac{\text{RGB Value (0--255)}}{255.0}$$

> **Example**: Sky Blue in RGB is `(135, 206, 235)`.  
> In OpenGL: `glColor3f(135.0f/255.0f, 206.0f/255.0f, 235.0f/255.0f);` $\rightarrow$ `glColor3f(0.53f, 0.81f, 0.92f);`

---

## 2. Solid Color Palette Table

| Color Name | Hex Code | `glColor3f(r, g, b)` | RGB (0–255) |
|---|---|---|---|
| **Black** | `#000000` | `glColor3f(0.0f, 0.0f, 0.0f);` | `(0, 0, 0)` |
| **White** | `#FFFFFF` | `glColor3f(1.0f, 1.0f, 1.0f);` | `(255, 255, 255)` |
| **Red** | `#FF0000` | `glColor3f(1.0f, 0.0f, 0.0f);` | `(255, 0, 0)` |
| **Green** | `#00FF00` | `glColor3f(0.0f, 1.0f, 0.0f);` | `(0, 255, 0)` |
| **Blue** | `#0000FF` | `glColor3f(0.0f, 0.0f, 1.0f);` | `(0, 0, 255)` |
| **Yellow** | `#FFFF00` | `glColor3f(1.0f, 1.0f, 0.0f);` | `(255, 255, 0)` |
| **Cyan** | `#00FFFF` | `glColor3f(0.0f, 1.0f, 1.0f);` | `(0, 255, 255)` |
| **Magenta** | `#FF00FF` | `glColor3f(1.0f, 0.0f, 1.0f);` | `(255, 0, 255)` |
| **Orange** | `#FFA500` | `glColor3f(1.0f, 0.65f, 0.0f);` | `(255, 165, 0)` |
| **Purple** | `#800080` | `glColor3f(0.5f, 0.0f, 0.5f);` | `(128, 0, 128)` |
| **Pink** | `#FFC0CB` | `glColor3f(1.0f, 0.75f, 0.8f);` | `(255, 192, 203)` |
| **Lime** | `#32CD32` | `glColor3f(0.2f, 0.8f, 0.2f);` | `(50, 205, 50)` |
| **Sky Blue** | `#87CEEB` | `glColor3f(0.53f, 0.81f, 0.92f);` | `(135, 206, 235)` |
| **Navy Blue** | `#000080` | `glColor3f(0.0f, 0.0f, 0.5f);` | `(0, 0, 128)` |
| **Gold** | `#FFD700` | `glColor3f(1.0f, 0.84f, 0.0f);` | `(255, 215, 0)` |
| **Teal** | `#008080` | `glColor3f(0.0f, 0.5f, 0.5f);` | `(0, 128, 128)` |
| **Brown** | `#8B4513` | `glColor3f(0.55f, 0.27f, 0.07f);` | `(139, 69, 19)` |
| **Light Gray** | `#D3D3D3` | `glColor3f(0.83f, 0.83f, 0.83f);` | `(211, 211, 211)` |
| **Dark Gray** | `#A9A9A9` | `glColor3f(0.3f, 0.3f, 0.3f);` | `(77, 77, 77)` |

---

## 3. Creating Color Gradients in OpenGL

OpenGL automatically interpolates colors smoothly across a primitive's surface when different vertices are given different `glColor3f()` values before their `glVertex2f()` calls.

### A. Linear Vertical Gradient (Quad / Rectangle)

```cpp
// Vertical Gradient (Red at Bottom to Blue at Top)
glBegin(GL_QUADS);
    glColor3f(1.0f, 0.0f, 0.0f); // Bottom-Left (Red)
    glVertex2f(-0.5f, -0.5f);

    glColor3f(1.0f, 0.0f, 0.0f); // Bottom-Right (Red)
    glVertex2f( 0.5f, -0.5f);

    glColor3f(0.0f, 0.0f, 1.0f); // Top-Right (Blue)
    glVertex2f( 0.5f,  0.5f);

    glColor3f(0.0f, 0.0f, 1.0f); // Top-Left (Blue)
    glVertex2f(-0.5f,  0.5f);
glEnd();
```

---

### B. Linear Horizontal Gradient (Quad / Rectangle)

```cpp
// Horizontal Gradient (Yellow on Left to Magenta on Right)
glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 0.0f); // Bottom-Left (Yellow)
    glVertex2f(-0.5f, -0.5f);

    glColor3f(1.0f, 0.0f, 1.0f); // Bottom-Right (Magenta)
    glVertex2f( 0.5f, -0.5f);

    glColor3f(1.0f, 0.0f, 1.0f); // Top-Right (Magenta)
    glVertex2f( 0.5f,  0.5f);

    glColor3f(1.0f, 1.0f, 0.0f); // Top-Left (Yellow)
    glVertex2f(-0.5f,  0.5f);
glEnd();
```

---

### C. Multi-Color Corner Gradient (Triangle / Quad)

```cpp
// 3-Corner Rainbow Triangle
glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f); // Corner 1 (Red)
    glVertex2f(-0.5f, -0.5f);

    glColor3f(0.0f, 1.0f, 0.0f); // Corner 2 (Green)
    glVertex2f( 0.5f, -0.5f);

    glColor3f(0.0f, 0.0f, 1.0f); // Corner 3 (Blue)
    glVertex2f( 0.0f,  0.5f);
glEnd();
```

---

### D. Radial Gradient Circle (`GL_TRIANGLE_FAN`)

```cpp
// Sunburst Circle (Bright Center fading to Dark Edge)
void drawRadialGradientCircle(float cx, float cy, float radius, int segments = 100) {
    glBegin(GL_TRIANGLE_FAN);
        // Center Vertex Color (Bright Yellow)
        glColor3f(1.0f, 1.0f, 0.2f);
        glVertex2f(cx, cy);

        // Boundary Vertices Color (Deep Purple)
        for (int i = 0; i <= segments; ++i) {
            glColor3f(0.5f, 0.0f, 0.5f);
            float theta = 2.0f * 3.14159f * float(i) / float(segments);
            float x = cx + radius * cosf(theta);
            float y = cy + radius * sinf(theta);
            glVertex2f(x, y);
        }
    glEnd();
}
```

---

## 4. Key Rules for `glColor3f`

1. **State Machine Concept**: Calling `glColor3f()` changes the *current color state*. All subsequent `glVertex2f()` calls will use this color until `glColor3f()` is called again.
2. **Order Matters**: Always set `glColor3f()` **before** calling `glVertex2f()` for a vertex.
3. **Background Color**: Use `glClearColor(r, g, b, alpha)` inside `init()` to set the canvas background color.
