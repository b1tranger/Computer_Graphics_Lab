# Implementation Plan: Interactive Spherical Triangle Viewer (Modern OpenGL)

## 1. Project Overview & Objectives
This project implements an interactive 3D viewer for a general spherical triangle on a unit sphere using **C++17 and Modern OpenGL (GLFW, GLAD, GLM)**, satisfying all requirements of the **Complex Engineering Problem (CEP)** assignment:
- **Surface Generation**: Recursive geodesic subdivision of spherical triangles, projection onto the unit sphere, and indexed rendering (`glDrawElements`) with an edge midpoint cache ensuring zero duplicated vertices.
- **Geodesic Edges**: Arc rendering via Great-Circle SLERP with distinct edge colors (red, green, gold) and complete prevention of z-fighting via `glPolygonOffset`.
- **Lighting & Shaders**: GLSL vertex and fragment shaders featuring model-view-projection transformations, analytic per-vertex spherical normals ($\vec{n} = \vec{v}$), and Blinn-Phong lighting with specular highlights.
- **Mathematical Analysis**: Analytic computation of interior angles ($\alpha, \beta, \gamma$), spherical excess ($E = \alpha+\beta+\gamma-\pi$), and area verification for the standard octant ($A=(1,0,0), B=(0,1,0), C=(0,0,1) \implies \text{Area} = \pi/2 \approx 1.5708$).
- **Interaction**: Arcball / virtual trackball camera rotation, mouse wheel zoom, runtime subdivision level adjustments (`+`/`-`), and wireframe toggle (`W`).
- **Question 2 Extensions**: Ray-sphere picking via mouse unprojection, degenerate input validation (collinear/antipodal checks), and smooth geometric SLERP morphing.
- **Visual Match**: Replicating the provided reference screenshot with unit sphere coordinate grid rings, 3D coordinate axes, and corner vertex markers.

---

## 2. Directory Structure & Architecture

```
Assignment/CEP/
├── Spherical_Triangle_Complex_Engineering_Problem.pdf
├── IMG-20261116-WA0003.pdf
└── SphericalTriangleViewer/
    ├── Makefile                  # Build script for MinGW-w64 (win) & Linux
    ├── include/
    │   ├── glad.h
    │   ├── glfw3.h
    │   └── glm/                  # GLM headers (copied from lab-8)
    ├── lib/
    │   ├── glfw3.dll             # Dynamic library for Windows execution
    │   └── libglfw3.a            # Static library for linking
    ├── shaders/
    │   ├── surface.vert          # Vertex shader for Blinn-Phong shaded surface
    │   ├── surface.frag          # Fragment shader with ambient, diffuse, specular
    │   ├── line.vert             # Vertex shader for edges, axes, and grid rings
    │   └── line.frag             # Fragment shader for uniform line coloring
    ├── src/
    │   ├── glad.c                # GLAD OpenGL loader source
    │   ├── Camera.h / .cpp       # Arcball / Orbit camera with zoom and pan
    │   ├── SphericalMath.h       # Vector algebra, SLERP, interior angles, excess
    │   ├── SphericalMesh.h/.cpp  # Recursive subdivision with midpoint hash map & EBO
    │   ├── SceneRenderer.h/.cpp  # Grid rings, axes, markers, edge arcs, and surface
    │   ├── RayPicker.h/.cpp      # Ray-sphere unprojection and intersection (Q2)
    │   └── main.cpp              # Application entry point, GLFW loop, and UI controls
    ├── build/                    # Compiled binary outputs (main.exe)
    └── report/
        ├── Report.md             # Half-page engineering trade-off report
        └── screenshot.png        # Render output matching reference
```

---

## 3. Mathematical Specifications

### 3.1 Unit Sphere Geometry & Vertex Normalization
For any points $A, B, C \in \mathbb{R}^3$, the vertices are normalized onto the unit 2-sphere $\mathbb{S}^2$:
$$\hat{A} = \frac{A}{\|A\|}, \quad \hat{B} = \frac{B}{\|B\|}, \quad \hat{C} = \frac{C}{\|C\|}$$

### 3.2 Interior Angle & Spherical Excess Calculation
At vertex $A$, the great-circle arcs connecting $A \to B$ and $A \to C$ lie on planes with normal vectors:
$$\vec{n}_{AB} = \frac{A \times B}{\|A \times B\|}, \quad \vec{n}_{AC} = \frac{A \times C}{\|A \times C\|}$$
The interior angle $\alpha$ at vertex $A$ is the dihedral angle between these two great circle planes:
$$\cos \alpha = \vec{n}_{AB} \cdot \vec{n}_{AC} \implies \alpha = \arccos\left(\text{clamp}(\vec{n}_{AB} \cdot \vec{n}_{AC}, -1.0, 1.0)\right)$$
Similarly, the interior angles $\beta$ and $\gamma$ at vertices $B$ and $C$ are:
$$\vec{n}_{BA} = \frac{B \times A}{\|B \times A\|}, \quad \vec{n}_{BC} = \frac{B \times C}{\|B \times C\|} \implies \beta = \arccos\left(\text{clamp}(\vec{n}_{BA} \cdot \vec{n}_{BC}, -1.0, 1.0)\right)$$
$$\vec{n}_{CA} = \frac{C \times A}{\|C \times A\|}, \quad \vec{n}_{CB} = \frac{C \times B}{\|C \times B\|} \implies \gamma = \arccos\left(\text{clamp}(\vec{n}_{CA} \cdot \vec{n}_{CB}, -1.0, 1.0)\right)$$

By **Girard's Theorem**, the area of a spherical triangle on a unit sphere ($R=1$) equals its spherical excess $E$:
$$\text{Area} = R^2 \cdot E = E = \alpha + \beta + \gamma - \pi$$

#### Verification for Standard Octant
For $A=(1,0,0), B=(0,1,0), C=(0,0,1)$:
- $\vec{n}_{AB} = (0, 0, 1)$, $\vec{n}_{AC} = (0, -1, 0) \implies \vec{n}_{AB} \cdot \vec{n}_{AC} = 0 \implies \alpha = \frac{\pi}{2} = 90^\circ$
- $\beta = \frac{\pi}{2} = 90^\circ$, $\gamma = \frac{\pi}{2} = 90^\circ$
- Spherical excess $E = \frac{\pi}{2} + \frac{\pi}{2} + \frac{\pi}{2} - \pi = \frac{\pi}{2} \approx 1.5707963\text{ rad}$
- Total sphere area is $4\pi$; one octant is $\frac{4\pi}{8} = \frac{\pi}{2}$, verifying exact correctness.

---

## 4. Algorithmic Implementation Steps

### Step 1: Recursive Geodesic Subdivision with Edge Midpoint Caching
To satisfy Requirement (a) (*indexed drawing, no duplicate vertices, no `glBegin/glEnd`*):
1. Represent the mesh with:
   - `std::vector<Vertex>` where `struct Vertex { glm::vec3 pos; glm::vec3 normal; };`
   - `std::vector<unsigned int>` for triangle element indices (EBO).
2. During each recursion step from depth $k$ to $k+1$:
   - For every edge between vertex index $i$ and index $j$, construct an edge key: `uint64_t key = (min(i, j) << 32) | max(i, j)`.
   - Maintain a hash table `std::unordered_map<uint64_t, unsigned int> midpointCache`.
   - If `key` exists in `midpointCache`, retrieve the already-created midpoint vertex index.
   - If not, compute the midpoint in 3D:
     $$\vec{m} = \frac{\vec{v}_i + \vec{v}_j}{\|\vec{v}_i + \vec{v}_j\|}$$
     Set normal $\vec{n} = \vec{m}$, insert into vertex vector, record its index in `midpointCache`.
   - Subdivide triangle $(v_1, v_2, v_3)$ into 4 child triangles:
     $$(v_1, m_{12}, m_{31}), \quad (v_2, m_{23}, m_{12}), \quad (v_3, m_{31}, m_{23}), \quad (m_{12}, m_{23}, m_{31})$$
3. Depth justification analysis:
   - **Depth 3**: 64 triangles, 45 unique vertices (faceting visible along silhouette).
   - **Depth 5**: 1,024 triangles, 561 unique vertices (smooth curvature, negligible memory < 30 KB).
   - **Depth 6**: 4,096 triangles, 2,145 unique vertices (flawless specular highlight, rendered in < 0.1 ms on integrated GPUs).

### Step 2: Great-Circle Geodesic Edges & Z-Fighting Elimination
To satisfy Requirement (b):
1. Sample each edge $A \to B, B \to C, C \to A$ into $M$ geodesic points ($M \ge 64$) using SLERP:
   $$\vec{p}(t) = \frac{\sin((1-t)\theta)}{\sin\theta} \vec{A} + \frac{\sin(t\theta)}{\sin\theta} \vec{B}, \quad \theta = \arccos(\vec{A} \cdot \vec{B}), \quad t \in [0, 1]$$
2. Set distinct color uniforms:
   - Edge $AB$: Red `(0.95, 0.25, 0.25)`
   - Edge $BC$: Green `(0.25, 0.85, 0.25)`
   - Edge $CA$: Gold/Yellow `(0.95, 0.85, 0.15)`
3. **Z-Fighting Prevention**:
   - Enable `glEnable(GL_POLYGON_OFFSET_FILL)`.
   - Call `glPolygonOffset(1.0f, 1.0f)` before rendering the spherical triangle surface, slightly shifting its depth values backward in the depth buffer.
   - Render great-circle line strips with `glLineWidth(3.0f)` or with a fractional radius offset ($R_{edge} = 1.002$), guaranteeing rock-solid edge visibility with zero depth contention.

### Step 3: Modern OpenGL Shaders & Blinn-Phong Lighting Pipeline
To satisfy Requirement (c):
1. **Vertex Shader**:
   ```glsl
   #version 330 core
   layout (location = 0) in vec3 aPos;
   layout (location = 1) in vec3 aNormal;
   uniform mat4 model;
   uniform mat4 view;
   uniform mat4 projection;
   out vec3 FragPos;
   out vec3 Normal;
   void main() {
       FragPos = vec3(model * vec4(aPos, 1.0));
       Normal = mat3(transpose(inverse(model))) * aNormal;
       gl_Position = projection * view * vec4(FragPos, 1.0);
   }
   ```
2. **Fragment Shader (Blinn-Phong)**:
   - Surface color: Vibrant Blue `vec3(0.18, 0.55, 0.95)` matching reference image.
   - Specular exponent: `shininess = 64.0`, producing the centered highlight visible in the prompt screenshot.
   - Halfway vector $\vec{H} = \text{normalize}(\vec{L} + \vec{V})$ prevents angular artifacts and guarantees smooth gradient shading.

### Step 4: Spherical Coordinate Grid & Scene Elements
To match the reference output (`IMG-20261116-WA0003.pdf`):
1. **Unit Sphere Coordinate Grid**:
   - Draw major great circles in XY, YZ, ZX planes (radius 1.0, subtle gray `vec3(0.4, 0.4, 0.4)`).
   - Draw intermediate latitude/longitude rings with fine line width.
2. **Coordinate Axes**:
   - Draw X, Y, Z axes from origin to length $1.6$ with thin white/gray lines.
3. **Vertex Markers**:
   - Render small white cubes or 3D point markers at vertices $A, B, C$.

### Step 5: Interactive Camera & Controls
To satisfy Requirement (e):
- **Arcball Rotation**: Mouse click-and-drag converts screen coordinates to a sphere vector and updates rotation matrix/quaternion, allowing seamless inspection from any angle.
- **Scroll Zoom**: Mouse scroll adjusts field-of-view (FOV: $20^\circ - 90^\circ$) or camera radius ($1.5 - 8.0$).
- **Subdivision Controls**: Press `+` / `=` to increment recursion depth (up to 7), `-` to decrement (down to 0).
- **Wireframe Toggle**: Press `W` to toggle `glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)`.
- **Octant Reset / Presets**: Press `O` for default octant, `1`, `2`, `3` for arbitrary test configurations.
- **Console HUD**: Displays live angles $\alpha, \beta, \gamma$, excess $E$, area, vertex count, and current FPS in real time.

### Step 6: Question 2 - Ray-Sphere Picking, Validation & Morphing
1. **Ray-Sphere Picking**:
   - Convert cursor $(x, y)$ to normalized device coordinates:
     $$x_{ndc} = \frac{2x}{W} - 1, \quad y_{ndc} = 1 - \frac{2y}{H}$$
   - Unproject near and far clipping plane points using `glm::inverse(projection * view)`.
   - Test ray intersection with unit sphere $\|O + t \vec{d}\|^2 = 1$.
   - Calculate closest positive intersection point $\vec{P} = O + t \vec{d}$, normalize to $\mathbb{S}^2$.
2. **Degenerate Input Detection**:
   - Check collinearity: $|A \cdot (B \times C)| < 10^{-4}$ (points lie on a single great circle).
   - Check antipodal condition: $A \cdot B \le -0.999$, etc.
   - Check coincident condition: $\|A - B\| < 10^{-3}$, etc.
   - If invalid, reject with explicit console error and ignore input.
3. **Smooth Morph Animation**:
   - Animate vertices from current positions to new picked points using SLERP over duration $\tau = 1.0\text{ s}$.
   - Re-tessellate and recalculate angles and area dynamically per frame.

---

## 5. Report & Deliverables (Submission Package)

1. **Trade-offs Report (Half a page / Markdown & PDF)**:
   - **Trade-off 1: Geometric Accuracy vs. Rendering Performance (Recursive Subdivision Depth)**.
     Evaluating vertex count $O(4^d)$ vs. silhouette fidelity and cache locality. Depth 5–6 achieves sub-pixel visual accuracy while maintaining 1000+ FPS on Intel HD graphics.
   - **Trade-off 2: Analytic vs. Numerical Shading (Smooth Spherical Normals vs. Flat Face Normals)**.
     Using analytic normals $\vec{n} = \vec{v}$ completely eliminates facet boundaries regardless of subdivision depth, contrasting with flat face normals.
   - **Trade-off 3: Geodesic Edge Z-Fighting Resolution (Polygon Offset vs. Radial Scaling)**.
     Standard `glPolygonOffset` avoids geometric distortion compared to radial expansion which causes parallax separation at glancing angles.
2. **Submission Archive**:
   - `.cpp` and `.h` source files
   - GLSL shader files (`.vert`, `.frag`)
   - `Makefile` and instructions
   - High-resolution screenshot of the program output matching `IMG-20261116-WA0003.pdf`
   - PDF report
   - Single combined ZIP archive ready for Google Classroom submission.

---

## 6. Execution Timeline & Verification Milestones

| Milestone | Objective | Verification Criteria |
| :--- | :--- | :--- |
| **M1: Core Setup** | Project structure, libraries (GLFW, GLAD, GLM), and Makefile | Clean compilation of basic window on Windows (`make win`) |
| **M2: Geometry Engine** | Recursive subdivision, midpoint hash map, and geodesic SLERP | Correct vertex and index generation with zero duplicates |
| **M3: Rendering Pipeline** | Shaders, Blinn-Phong lighting, coordinate grid, and axis lines | Output matches visual appearance of reference screenshot |
| **M4: Mathematical Engine** | Vector angles, dihedral normals, spherical excess, and area | Octant produces exactly $\alpha=\beta=\gamma=90^\circ$, Area $=\pi/2$ |
| **M5: Interaction** | Arcball rotation, mouse zoom, runtime subdivision, wireframe | Responsive 60+ FPS interaction without camera gimbal lock |
| **M6: Extension (Q2)** | Ray-sphere picking, degenerate rejection, and SLERP morphing | User clicks 3 points on sphere to define new animated triangle |
| **M7: Final Packaging** | Documentation, trade-offs report, screenshot, and ZIP bundle | Complete package compliant with assignment submission rules |
