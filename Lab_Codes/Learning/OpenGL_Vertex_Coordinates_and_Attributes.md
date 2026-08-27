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

---

## 3. Rendering Multiple Objects with Distinct Shaders (Multi-VAO & Multi-Program)

When drawing multiple objects with independent geometries and colors, you can use multiple **Vertex Array Objects (VAOs)** and multiple **Shader Programs**.

### Complete Workflow to Add a New Colored Object

#### Step 1: Define Fragment Shader Source
```cpp
const char *fragmentShader3Source =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n" // Solid Red
    "}\n\0";
```

#### Step 2: Create, Compile, and Link the Shader Program
A single compiled vertex shader can be shared across multiple shader programs if their inputs/outputs match:

```cpp
// 1. Create shader objects
unsigned int fragmentShaderRed = glCreateShader(GL_FRAGMENT_SHADER);
unsigned int shaderProgramRed  = glCreateProgram();

// 2. Supply GLSL source and compile
glShaderSource(fragmentShaderRed, 1, &fragmentShader3Source, NULL);
glCompileShader(fragmentShaderRed);

// 3. Attach shared vertex shader + new fragment shader, then link
glAttachShader(shaderProgramRed, vertexShader);
glAttachShader(shaderProgramRed, fragmentShaderRed);
glLinkProgram(shaderProgramRed);
```

> **Warning**: Never skip `glShaderSource()` and `glCompileShader()` before `glLinkProgram()`. Without compilation, the shader object is empty and linking will fail silently without an explicit error check.

#### Step 3: Configure Array Buffers (VAO/VBO)
Allocate sufficient slots for all objects:
```cpp
unsigned int VBOs[3], VAOs[3];
glGenVertexArrays(3, VAOs);
glGenBuffers(3, VBOs);

// Configure the 3rd object (Index 2)
glBindVertexArray(VAOs[2]);
glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
glBufferData(GL_ARRAY_BUFFER, sizeof(thirdTriangle), thirdTriangle, GL_STATIC_DRAW);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
glEnableVertexAttribArray(0);
```

#### Step 4: Render in the Render Loop
Switch shader programs and bind corresponding VAOs per draw call:
```cpp
// Draw Object 1 (Orange)
glUseProgram(shaderProgramOrange);
glBindVertexArray(VAOs[0]);
glDrawArrays(GL_TRIANGLES, 0, 3);

// Draw Object 2 (Yellow)
glUseProgram(shaderProgramYellow);
glBindVertexArray(VAOs[1]);
glDrawArrays(GL_TRIANGLES, 0, 3);

// Draw Object 3 (Red)
glUseProgram(shaderProgramRed);
glBindVertexArray(VAOs[2]);
glDrawArrays(GL_TRIANGLES, 0, 3);
```

#### Step 5: Clean Up All Allocated Resources
Make sure the count passed to `glDelete*` matches the total allocated:
```cpp
glDeleteVertexArrays(3, VAOs);
glDeleteBuffers(3, VBOs);
glDeleteProgram(shaderProgramOrange);
glDeleteProgram(shaderProgramYellow);
glDeleteProgram(shaderProgramRed);
```
