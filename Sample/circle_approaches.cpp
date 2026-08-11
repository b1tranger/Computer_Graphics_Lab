#include <iostream>
#include <cmath>
#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// =========================================================================
// APPROACH 1: Direct Manual Vertices (Like cgm lab-3.1 basic shapes)
// =========================================================================
void drawManualOctagonCircle(float cx, float cy, float radius) {
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.3f, 0.3f); // Red
        glVertex2f(cx + 0.00f * radius, cy + 1.00f * radius); // Top
        glVertex2f(cx + 0.71f * radius, cy + 0.71f * radius); // Top-Right
        glVertex2f(cx + 1.00f * radius, cy + 0.00f * radius); // Right
        glVertex2f(cx + 0.71f * radius, cy - 0.71f * radius); // Bottom-Right
        glVertex2f(cx + 0.00f * radius, cy - 1.00f * radius); // Bottom
        glVertex2f(cx - 0.71f * radius, cy - 0.71f * radius); // Bottom-Left
        glVertex2f(cx - 1.00f * radius, cy + 0.00f * radius); // Left
        glVertex2f(cx - 0.71f * radius, cy + 0.71f * radius); // Top-Left
    glEnd();
}

// =========================================================================
// APPROACH 2: Simple Degree-Based Loop (0 to 360 Degrees)
// =========================================================================
void drawDegreeLoopCircle(float cx, float cy, float radius) {
    glBegin(GL_POLYGON);
        glColor3f(0.3f, 0.7f, 1.0f); // Blue
        for (float angle = 0.0f; angle < 360.0f; angle += 10.0f) {
            float rad = angle * (M_PI / 180.0f);
            float x = cx + radius * cos(rad);
            float y = cy + radius * sin(rad);
            glVertex2f(x, y);
        }
    glEnd();
}

// =========================================================================
// APPROACH 3: Smooth Trigonometric Loop (GL_TRIANGLE_FAN / GL_LINE_LOOP)
// =========================================================================
void drawSmoothTrigCircle(float cx, float cy, float radius, int segments = 100) {
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(0.3f, 0.9f, 0.4f); // Green
        glVertex2f(cx, cy);
        for (int i = 0; i <= segments; ++i) {
            float theta = 2.0f * M_PI * float(i) / float(segments);
            glVertex2f(cx + radius * cosf(theta), cy + radius * sinf(theta));
        }
    glEnd();
}

// =========================================================================
// DISPLAY FUNCTION
// =========================================================================
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Left: Approach 1 - Direct Manual Vertices (Octagon)
    drawManualOctagonCircle(-0.6f, 0.0f, 0.35f);

    // Middle: Approach 2 - Simple Degree-Based Loop
    drawDegreeLoopCircle(0.0f, 0.0f, 0.35f);

    // Right: Approach 3 - Smooth Trigonometric Fan Loop
    drawSmoothTrigCircle(0.6f, 0.0f, 0.35f);

    glFlush();
}

void init() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 400);
    glutInitWindowPosition(150, 150);
    glutCreateWindow("Circle Drawing Approaches - Comparison");

    init();
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}
