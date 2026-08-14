#include <iostream>
#include <cmath>
#include <string>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// =========================================================================
// HELPER DRAWING FUNCTIONS FOR TEXT & GRID
// =========================================================================

// Utility to render bitmap text string on screen
void drawText(float x, float y, const std::string& text, void* font = GLUT_BITMAP_HELVETICA_12) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}

// Utility to draw grid section borders and titles
void drawSectionGrid() {
    glLineWidth(1.0f);
    glColor3f(0.3f, 0.3f, 0.35f); // Subtle grid lines

    // Vertical dividing line (X = 0)
    glBegin(GL_LINES);
        glVertex2f(0.0f, -1.0f);
        glVertex2f(0.0f, 1.0f);
    glEnd();

    // Horizontal dividing lines (Y = 0.33, Y = -0.33)
    glBegin(GL_LINES);
        glVertex2f(-1.0f, 0.33f);
        glVertex2f( 1.0f, 0.33f);

        glVertex2f(-1.0f, -0.33f);
        glVertex2f( 1.0f, -0.33f);
    glEnd();
}

// =========================================================================
// BORDER TYPE DEMONSTRATIONS
// =========================================================================

// 1. Basic Line Loop Borders with Different Thicknesses (Top-Left)
void drawSection1_LineThickness() {
    // Title
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(-0.95f, 0.90f, "1. Line Widths (GL_LINE_LOOP)");

    // Thin Border (1.0px)
    glLineWidth(1.0f);
    glColor3f(0.2f, 0.8f, 1.0f); // Sky Blue
    glBegin(GL_LINE_LOOP);
        glVertex2f(-0.85f, 0.45f);
        glVertex2f(-0.55f, 0.45f);
        glVertex2f(-0.55f, 0.80f);
        glVertex2f(-0.85f, 0.80f);
    glEnd();
    drawText(-0.82f, 0.40f, "Width: 1.0px");

    // Medium Border (3.0px)
    glLineWidth(3.0f);
    glColor3f(0.2f, 1.0f, 0.4f); // Green
    glBegin(GL_LINE_LOOP);
        glVertex2f(-0.45f, 0.45f);
        glVertex2f(-0.15f, 0.45f);
        glVertex2f(-0.15f, 0.80f);
        glVertex2f(-0.45f, 0.80f);
    glEnd();
    drawText(-0.42f, 0.40f, "Width: 3.0px");

    // Thick Triangle Border (6.0px)
    glLineWidth(6.0f);
    glColor3f(1.0f, 0.4f, 0.2f); // Orange
    glBegin(GL_LINE_LOOP);
        glVertex2f(-0.80f, 0.45f - 0.35f);
        glVertex2f(-0.20f, 0.45f - 0.35f);
        glVertex2f(-0.50f, 0.75f - 0.35f);
    glEnd();

    glLineWidth(1.0f); // Reset
}

// 2. Dashed & Dotted Patterned Borders (Top-Right)
void drawSection2_StippledBorders() {
    // Title
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(0.05f, 0.90f, "2. Stippled Borders (glLineStipple)");

    glEnable(GL_LINE_STIPPLE);
    glLineWidth(2.5f);

    // A. Dashed Border (Pattern 0x00FF)
    glLineStipple(1, 0x00FF);
    glColor3f(1.0f, 0.3f, 0.3f); // Red
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.10f, 0.65f);
        glVertex2f(0.45f, 0.65f);
        glVertex2f(0.45f, 0.85f);
        glVertex2f(0.10f, 0.85f);
    glEnd();
    drawText(0.12f, 0.60f, "Dashed (0x00FF)");

    // B. Dotted Border (Pattern 0x5555)
    glLineStipple(2, 0x5555);
    glColor3f(1.0f, 0.9f, 0.2f); // Yellow
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.55f, 0.65f);
        glVertex2f(0.90f, 0.65f);
        glVertex2f(0.90f, 0.85f);
        glVertex2f(0.55f, 0.85f);
    glEnd();
    drawText(0.57f, 0.60f, "Dotted (0x5555)");

    // C. Dash-Dot Circle Border
    glLineStipple(1, 0x1C47);
    glColor3f(0.8f, 0.4f, 1.0f); // Purple
    float cx = 0.50f, cy = 0.44f, r = 0.12f;
    glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 60; ++i) {
            float theta = 2.0f * M_PI * i / 60.0f;
            glVertex2f(cx + r * cosf(theta), cy + r * sinf(theta));
        }
    glEnd();
    drawText(0.35f, 0.36f, "Dash-Dot Circle (0x1C47)");

    glDisable(GL_LINE_STIPPLE);
    glLineWidth(1.0f);
}

// 3. Multi-Color & Gradient Borders (Middle-Left)
void drawSection3_GradientBorders() {
    // Title
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(-0.95f, 0.25f, "3. Gradient & Multi-Color Borders");

    // Smooth Gradient Rectangle Border
    glLineWidth(4.0f);
    glBegin(GL_LINE_LOOP);
        glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(-0.85f, -0.25f); // Red
        glColor3f(0.0f, 1.0f, 0.0f); glVertex2f(-0.55f, -0.25f); // Green
        glColor3f(0.0f, 0.0f, 1.0f); glVertex2f(-0.55f,  0.15f); // Blue
        glColor3f(1.0f, 1.0f, 0.0f); glVertex2f(-0.85f,  0.15f); // Yellow
    glEnd();
    drawText(-0.85f, -0.30f, "4-Corner Gradient");

    // Per-Segment Colored Hexagon Border
    glLineWidth(3.0f);
    float hcx = -0.30f, hcy = -0.05f, hr = 0.18f;
    glBegin(GL_LINE_LOOP);
        glColor3f(1.0f, 0.2f, 0.2f); glVertex2f(hcx + hr * cosf(0 * M_PI / 3), hcy + hr * sinf(0 * M_PI / 3));
        glColor3f(1.0f, 0.6f, 0.0f); glVertex2f(hcx + hr * cosf(1 * M_PI / 3), hcy + hr * sinf(1 * M_PI / 3));
        glColor3f(0.2f, 0.9f, 0.2f); glVertex2f(hcx + hr * cosf(2 * M_PI / 3), hcy + hr * sinf(2 * M_PI / 3));
        glColor3f(0.0f, 0.8f, 1.0f); glVertex2f(hcx + hr * cosf(3 * M_PI / 3), hcy + hr * sinf(3 * M_PI / 3));
        glColor3f(0.6f, 0.3f, 1.0f); glVertex2f(hcx + hr * cosf(4 * M_PI / 3), hcy + hr * sinf(4 * M_PI / 3));
        glColor3f(1.0f, 0.4f, 0.8f); glVertex2f(hcx + hr * cosf(5 * M_PI / 3), hcy + hr * sinf(5 * M_PI / 3));
    glEnd();
    drawText(-0.42f, -0.30f, "Rainbow Hexagon");

    glLineWidth(1.0f);
}

// 4. Combined Solid Fill + Outer Border (Middle-Right)
void drawSection4_FillWithBorder() {
    // Title
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(0.05f, 0.25f, "4. Solid Fill + Outer Border");

    // Shape A: Filled Square with Dark Outline
    // Pass 1: Solid Fill
    glColor3f(1.0f, 0.85f, 0.2f); // Gold Yellow Fill
    glBegin(GL_QUADS);
        glVertex2f(0.10f, -0.22f);
        glVertex2f(0.45f, -0.22f);
        glVertex2f(0.45f,  0.15f);
        glVertex2f(0.10f,  0.15f);
    glEnd();
    // Pass 2: Thick Outer Border
    glLineWidth(4.0f);
    glColor3f(0.5f, 0.1f, 0.0f); // Dark Red-Brown Border
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.10f, -0.22f);
        glVertex2f(0.45f, -0.22f);
        glVertex2f(0.45f,  0.15f);
        glVertex2f(0.10f,  0.15f);
    glEnd();
    drawText(0.12f, -0.29f, "Fill + Solid Border");

    // Shape B: Filled Circle with Dashed Outer Border
    float ccx = 0.72f, ccy = -0.04f, cr = 0.18f;
    // Pass 1: Light Cyan Fill
    glColor3f(0.6f, 0.95f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(ccx, ccy);
        for (int i = 0; i <= 60; ++i) {
            float t = 2.0f * M_PI * i / 60.0f;
            glVertex2f(ccx + cr * cosf(t), ccy + cr * sinf(t));
        }
    glEnd();
    // Pass 2: Navy Blue Dashed Border
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x00FF);
    glLineWidth(3.0f);
    glColor3f(0.0f, 0.15f, 0.6f);
    glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 60; ++i) {
            float t = 2.0f * M_PI * i / 60.0f;
            glVertex2f(ccx + cr * cosf(t), ccy + cr * sinf(t));
        }
    glEnd();
    glDisable(GL_LINE_STIPPLE);
    drawText(0.55f, -0.29f, "Fill + Dashed Border");

    glLineWidth(1.0f);
}

// 5. Wireframe Mode via glPolygonMode (Bottom-Left)
void drawSection5_WireframeMode() {
    // Title
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(-0.95f, -0.40f, "5. Wireframe Mode (glPolygonMode)");

    // Switch OpenGL to wireframe rendering mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.0f);

    // Polygon 1: Wireframe Star / Octagon
    glColor3f(0.2f, 0.9f, 0.9f); // Cyan
    float cx1 = -0.70f, cy1 = -0.68f, r1 = 0.22f;
    glBegin(GL_POLYGON);
        for (int i = 0; i < 8; ++i) {
            float t = i * M_PI / 4.0f;
            glVertex2f(cx1 + r1 * cosf(t), cy1 + r1 * sinf(t));
        }
    glEnd();
    drawText(-0.85f, -0.96f, "glPolygonMode(..., GL_LINE)");

    // Polygon 2: Wireframe House Shape
    glColor3f(1.0f, 0.4f, 0.8f); // Pink wireframe
    glBegin(GL_QUADS);
        glVertex2f(-0.35f, -0.85f);
        glVertex2f(-0.10f, -0.85f);
        glVertex2f(-0.10f, -0.60f);
        glVertex2f(-0.35f, -0.60f);
    glEnd();
    glBegin(GL_TRIANGLES);
        glVertex2f(-0.38f, -0.60f);
        glVertex2f(-0.07f, -0.60f);
        glVertex2f(-0.225f, -0.45f);
    glEnd();

    // Revert back to GL_FILL mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(1.0f);
}

// 6. Open Arc & Concentric Ring Borders (Bottom-Right)
void drawSection6_ArcsAndRings() {
    // Title
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(0.05f, -0.40f, "6. Open Arcs & Concentric Rings");

    // Open Arc Border using GL_LINE_STRIP
    glLineWidth(3.0f);
    glColor3f(1.0f, 0.5f, 0.0f); // Orange Arc
    float acx = 0.25f, acy = -0.68f, ar = 0.20f;
    glBegin(GL_LINE_STRIP); // Leaves end points unconnected
        for (int i = 0; i <= 40; ++i) {
            float angleDeg = 30.0f + (210.0f - 30.0f) * ((float)i / 40.0f);
            float rad = angleDeg * (M_PI / 180.0f);
            glVertex2f(acx + ar * cosf(rad), acy + ar * sinf(rad));
        }
    glEnd();
    drawText(0.10f, -0.96f, "Open Arc (GL_LINE_STRIP)");

    // Concentric Circular Ring Borders
    float rcx = 0.72f, rcy = -0.68f;
    float ringRadii[] = { 0.08f, 0.14f, 0.20f };
    float ringWidths[] = { 1.5f, 3.0f, 4.5f };
    float ringColors[3][3] = {
        {1.0f, 0.2f, 0.4f},
        {0.2f, 0.8f, 1.0f},
        {0.4f, 1.0f, 0.4f}
    };

    for (int rIdx = 0; rIdx < 3; ++rIdx) {
        glLineWidth(ringWidths[rIdx]);
        glColor3fv(ringColors[rIdx]);
        glBegin(GL_LINE_LOOP);
            for (int i = 0; i < 50; ++i) {
                float t = 2.0f * M_PI * i / 50.0f;
                glVertex2f(rcx + ringRadii[rIdx] * cosf(t), rcy + ringRadii[rIdx] * sinf(t));
            }
        glEnd();
    }
    drawText(0.58f, -0.96f, "Concentric Rings");

    glLineWidth(1.0f);
}

// =========================================================================
// MAIN DISPLAY & GLUT CALLBACKS
// =========================================================================

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw background section grid
    drawSectionGrid();

    // Render all 6 border demonstration sections
    drawSection1_LineThickness();
    drawSection2_StippledBorders();
    drawSection3_GradientBorders();
    drawSection4_FillWithBorder();
    drawSection5_WireframeMode();
    drawSection6_ArcsAndRings();

    glFlush();
}

void init() {
    // Dark Charcoal background color for crisp border visibility
    glClearColor(0.12f, 0.12f, 0.15f, 1.0f);

    // Set 2D Orthographic Projection Matrix (-1.0 to 1.0 on both axes)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(900, 700);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Border Practice Showcase - OpenGL GLUT (border_prac)");

    init();
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}
