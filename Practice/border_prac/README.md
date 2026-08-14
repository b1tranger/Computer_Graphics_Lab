# Border Practice Project (`border_prac`)

A comprehensive OpenGL GLUT C++ practice application exploring different techniques, primitive types, line styles, and color combinations for rendering **colored borders and hollow shapes**.

---

## Features Demonstrated

The application divides the screen into 6 interactive demonstration sections:

1. **Line Thickness (`GL_LINE_LOOP` & `glLineWidth`)**:
   - Compares 1.0px (thin), 3.0px (medium), and 6.0px (thick) line widths on rectangles and triangles.
2. **Stippled / Patterned Borders (`glLineStipple`)**:
   - Dashed borders (`0x00FF`)
   - Dotted borders (`0x5555`)
   - Dash-Dot circular borders (`0x1C47`)
3. **Gradient & Multi-Color Borders**:
   - Smooth 4-corner color interpolations across vertices (Red $\rightarrow$ Green $\rightarrow$ Blue $\rightarrow$ Yellow).
   - Per-segment multi-colored rainbow hexagon border.
4. **Solid Fill + Outer Border (Two-Pass Rendering)**:
   - Gold yellow solid fill with dark red-brown thick outer border.
   - Light cyan circular fill with navy blue dashed outer border.
5. **Wireframe Mode (`glPolygonMode`)**:
   - Toggling global polygon rendering to wireframe using `glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)`.
6. **Open Arcs & Concentric Rings**:
   - Unclosed open arcs using `GL_LINE_STRIP`.
   - Concentric circular rings with varying line widths and color palettes.

---

## How to Run in CodeBlocks

1. Open **`border_prac.cbp`** in Code::Blocks.
2. Ensure your target compiler is set to MinGW GCC with freeglut/glut32 linked.
3. Click **Build and Run** (`F9`).
