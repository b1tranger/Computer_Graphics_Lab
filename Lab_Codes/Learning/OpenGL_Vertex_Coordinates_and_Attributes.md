# OpenGL Learning Notes: Coordinates, Winding, and Vertex Attributes

## 1. Normalized Device Coordinates (NDC) & Triangle Orientation

### Coordinate System in OpenGL
OpenGL NDC coordinates range from `-1.0` to `+1.0` along both the X and Y axes:
* **X-axis**: `-1.0` (Left) $\to$ `+1.0` (Right)
* **Y-axis**: `-1.0` (**Bottom**) $\to$ `+1.0` (**Top**)
* **Origin `(0.0, 0.0)`**: Center of the viewport/screen.

```
            (+1.0) Y [Top]
               ▲
               │
 (-1.0) ───────┼─────── (+1.0) X [Right]
 [Left]        │
               │
               ▼
            (-1.0) Y [Bottom]
```

---

### Inverted Triangle vs. Upright Triangle

#### Inverted Triangle (Apex pointing Down)
```cpp
float vertices[] = {
    -0.5f,  0.5f, 0.0f, // Top-Left
     0.5f,  0.5f, 0.0f, // Top-Right
     0.0f, -0.5f, 0.0f  // Bottom-Center (Apex)
};
```

**Visual:**
```
(-0.5, 0.5) •────────────• (0.5, 0.5)   <-- Base at Y = +0.5 (Top)
             \          /
              \        /
               \      /
                \    /
                  •• (0.0, -0.5)        <-- Apex at Y = -0.5 (Bottom)
```

#### Upright Triangle (Apex pointing Up)
```cpp
float vertices[] = {
    -0.5f, -0.5f, 0.0f, // Bottom-Left
     0.5f, -0.5f, 0.0f, // Bottom-Right
     0.0f,  0.5f, 0.0f  // Top-Center (Apex)
};
```

---

## 2. Vertex Data Dimensions (Are `0.0f` Z-coordinates needed?)

The `0.0f` represents the **Z-coordinate (depth)** in 3D $(X, Y, Z)$ coordinates.

### When are they required?
In the standard template, they are **required** because:
1. **`glVertexAttribPointer`** specifies `size = 3` and `stride = 3 * sizeof(float)`:
   ```cpp
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
   ```
2. **Vertex Shader** expects a 3-component vector `vec3`:
   ```glsl
   #version 330 core
   layout (location = 0) in vec3 aPos;
   void main()
   {
       gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
   }
   ```
If you simply delete `0.0f` from `vertices[]` without changing the pointer setup, OpenGL will misalign the memory layout and read invalid bytes.

---

### How to use purely 2D Coordinates (Removing `0.0f`)

If you want to remove the Z-coordinates for 2D rendering:

1. **Modify Vertex Array:**
   ```cpp
   float vertices[] = {
       -0.5f,  0.5f, // Top-Left (X, Y)
        0.5f,  0.5f, // Top-Right (X, Y)
        0.0f, -0.5f  // Bottom-Center (X, Y)
   };
   ```

2. **Update Vertex Attribute Pointer:**
   Set `size = 2` and `stride = 2 * sizeof(float)`:
   ```cpp
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
   ```

3. **Update Vertex Shader:**
   Accept `vec2` and supply `0.0` for Z in `gl_Position`:
   ```glsl
   #version 330 core
   layout (location = 0) in vec2 aPos;
   void main()
   {
       gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
   }
   ```
