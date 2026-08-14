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

## 2. Solid Color Palette Tables (Categorized by Shade)

### A. Greens (Light to Dark)

| Color Name | Hex Code | `glColor3f(r, g, b)` | RGB (0–255) | Description / Usage |
|---|---|---|---|---|
| **Pale Green** | `#98FB98` | `glColor3f(0.60f, 0.98f, 0.60f);` | `(152, 251, 152)` | Soft background / highlights |
| **Light Green** | `#90EE90` | `glColor3f(0.56f, 0.93f, 0.56f);` | `(144, 238, 144)` | Meadow / light grass |
| **Lime** | `#00FF00` | `glColor3f(0.0f, 1.0f, 0.0f);` | `(0, 255, 0)` | Pure neon green |
| **Lime Green** | `#32CD32` | `glColor3f(0.20f, 0.80f, 0.20f);` | `(50, 205, 50)` | Standard vibrant foliage |
| **Green (Medium)** | `#008000` | `glColor3f(0.0f, 0.50f, 0.0f);` | `(0, 128, 0)` | Standard dark grass |
| **Forest Green** | `#228B22` | `glColor3f(0.13f, 0.55f, 0.13f);` | `(34, 139, 34)` | Tree leaves / vegetation |
| **Olive Green** | `#556B2F` | `glColor3f(0.33f, 0.42f, 0.18f);` | `(85, 107, 47)` | Camouflage / dark moss |
| **Dark Green** | `#006400` | `glColor3f(0.0f, 0.39f, 0.0f);` | `(0, 100, 0)` | Deep shadows / night trees |

### B. Reds (Light to Dark)

| Color Name | Hex Code | `glColor3f(r, g, b)` | RGB (0–255) | Description / Usage |
|---|---|---|---|---|
| **Light Coral / Light Red** | `#F08080` | `glColor3f(0.94f, 0.50f, 0.50f);` | `(240, 128, 128)` | Soft red highlight |
| **Red (Pure)** | `#FF0000` | `glColor3f(1.0f, 0.0f, 0.0f);` | `(255, 0, 0)` | Primary red / warning |
| **Crimson** | `#DC143C` | `glColor3f(0.86f, 0.08f, 0.24f);` | `(220, 20, 60)` | Deep vibrant red |
| **Dark Red** | `#8B0000` | `glColor3f(0.55f, 0.0f, 0.0f);` | `(139, 0, 0)` | Deep red shadow |
| **Maroon** | `#800000` | `glColor3f(0.50f, 0.0f, 0.0f);` | `(128, 0, 0)` | Very dark reddish brown |

### C. Blues (Light to Dark)

| Color Name | Hex Code | `glColor3f(r, g, b)` | RGB (0–255) | Description / Usage |
|---|---|---|---|---|
| **Light Sky Blue** | `#87CEFA` | `glColor3f(0.53f, 0.81f, 0.98f);` | `(135, 206, 250)` | Daytime sky |
| **Sky Blue** | `#87CEEB` | `glColor3f(0.53f, 0.81f, 0.92f);` | `(135, 206, 235)` | Standard clear sky |
| **Blue (Pure)** | `#0000FF` | `glColor3f(0.0f, 0.0f, 1.0f);` | `(0, 0, 255)` | Primary blue |
| **Royal Blue** | `#4169E1` | `glColor3f(0.25f, 0.41f, 0.88f);` | `(65, 105, 225)` | Bright deep blue |
| **Navy Blue** | `#000080` | `glColor3f(0.0f, 0.0f, 0.50f);` | `(0, 0, 128)` | Dark sea / night sky |
| **Midnight Blue** | `#191970` | `glColor3f(0.10f, 0.10f, 0.44f);` | `(25, 25, 112)` | Deep night background |

### D. Yellows & Oranges (Light to Dark)

| Color Name | Hex Code | `glColor3f(r, g, b)` | RGB (0–255) | Description / Usage |
|---|---|---|---|---|
| **Light Yellow** | `#FFFFE0` | `glColor3f(1.0f, 1.0f, 0.88f);` | `(255, 255, 224)` | Sunlight / cream |
| **Yellow (Pure)** | `#FFFF00` | `glColor3f(1.0f, 1.0f, 0.0f);` | `(255, 255, 0)` | Primary yellow |
| **Gold** | `#FFD700` | `glColor3f(1.0f, 0.84f, 0.0f);` | `(255, 215, 0)` | Coins / warm sun |
| **Peach** | `#FFDAB9` | `glColor3f(1.0f, 0.85f, 0.73f);` | `(255, 218, 185)` | Skin tone / soft highlight |
| **Orange** | `#FFA500` | `glColor3f(1.0f, 0.65f, 0.0f);` | `(255, 165, 0)` | Sunset / fruit |
| **Dark Orange** | `#FF8C00` | `glColor3f(1.0f, 0.55f, 0.0f);` | `(255, 140, 0)` | Autumn leaves |

### E. Purples & Pinks (Light to Dark)

| Color Name | Hex Code | `glColor3f(r, g, b)` | RGB (0–255) | Description / Usage |
|---|---|---|---|---|
| **Lavender** | `#E6E6FA` | `glColor3f(0.90f, 0.90f, 0.98f);` | `(230, 230, 250)` | Soft light purple |
| **Light Pink** | `#FFB6C1` | `glColor3f(1.0f, 0.71f, 0.76f);` | `(255, 182, 193)` | Blossom pink |
| **Pink** | `#FFC0CB` | `glColor3f(1.0f, 0.75f, 0.80f);` | `(255, 192, 203)` | Standard pink |
| **Hot Pink** | `#FF69B4` | `glColor3f(1.0f, 0.41f, 0.71f);` | `(255, 105, 180)` | Vibrant pink |
| **Magenta** | `#FF00FF` | `glColor3f(1.0f, 0.0f, 1.0f);` | `(255, 0, 255)` | Pure fuchsia / cyan pair |
| **Purple** | `#800080` | `glColor3f(0.50f, 0.0f, 0.50f);` | `(128, 0, 128)` | Standard purple |
| **Dark Purple / Indigo** | `#4B0082` | `glColor3f(0.29f, 0.0f, 0.51f);` | `(75, 0, 130)` | Deep night sky / magic |

### F. Cyans & Teals (Light to Dark)

| Color Name | Hex Code | `glColor3f(r, g, b)` | RGB (0–255) | Description / Usage |
|---|---|---|---|---|
| **Light Cyan** | `#E0FFFF` | `glColor3f(0.88f, 1.0f, 1.0f);` | `(224, 255, 255)` | Ice / soft water |
| **Cyan (Pure)** | `#00FFFF` | `glColor3f(0.0f, 1.0f, 1.0f);` | `(0, 255, 255)` | Bright neon turquoise |
| **Turquoise** | `#40E0D0` | `glColor3f(0.25f, 0.88f, 0.82f);` | `(64, 224, 208)` | Tropical water |
| **Teal (Dark Cyan)** | `#008080` | `glColor3f(0.0f, 0.50f, 0.50f);` | `(0, 128, 128)` | Deep water / shadow |

### G. Browns & Earth Tones (Light to Dark)

| Color Name | Hex Code | `glColor3f(r, g, b)` | RGB (0–255) | Description / Usage |
|---|---|---|---|---|
| **Khaki / Tan** | `#F0E68C` | `glColor3f(0.94f, 0.90f, 0.55f);` | `(240, 230, 140)` | Sand / ground |
| **Wood Brown** | `#CD853F` | `glColor3f(0.80f, 0.52f, 0.25f);` | `(205, 133, 63)` | Wooden stems / fences |
| **Saddle Brown** | `#8B4513` | `glColor3f(0.55f, 0.27f, 0.07f);` | `(139, 69, 19)` | Tree trunks / soil |
| **Dark Brown** | `#3E2723` | `glColor3f(0.24f, 0.15f, 0.14f);` | `(62, 39, 35)` | Bark / deep earth shadow |

### H. Grays & Monochrome (Light to Dark)

| Color Name | Hex Code | `glColor3f(r, g, b)` | RGB (0–255) | Description / Usage |
|---|---|---|---|---|
| **White** | `#FFFFFF` | `glColor3f(1.0f, 1.0f, 1.0f);` | `(255, 255, 255)` | Clouds / snow / bright light |
| **Light Gray** | `#D3D3D3` | `glColor3f(0.83f, 0.83f, 0.83f);` | `(211, 211, 211)` | Light metallic / stone |
| **Gray / Medium Gray** | `#808080` | `glColor3f(0.50f, 0.50f, 0.50f);` | `(128, 128, 128)` | Concrete / neutral shadow |
| **Dark Gray** | `#404040` | `glColor3f(0.25f, 0.25f, 0.25f);` | `(64, 64, 64)` | Dark stone / road |
| **Charcoal** | `#222222` | `glColor3f(0.13f, 0.13f, 0.13f);` | `(34, 34, 34)` | Almost black / asphalt |
| **Black** | `#000000` | `glColor3f(0.0f, 0.0f, 0.0f);` | `(0, 0, 0)` | Pure black / void |

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
