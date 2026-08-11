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
// METHOD 1: Direct Manual Vertices (Simple approach like cgm lab-3.1)
// Writing individual glVertex2f calls around the perimeter
// =========================================================================
void drawDirectManualCircle() {
    // A circle approximated by an 8-sided polygon (Octagon) using GL_POLYGON
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.2f, 0.2f); // Red fill
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

// =========================================================================
// METHOD 2: Simple Degree-based Loop (0 to 360 degrees)
// Beginner-friendly loop using degrees instead of radians
// =========================================================================
void drawSimpleDegreeCircle(float cx, float cy, float radius) {
    glBegin(GL_POLYGON); // Can also use GL_LINE_LOOP for outline
        glColor3f(0.2f, 0.6f, 1.0f); // Blue fill
        for (float angle = 0.0f; angle < 360.0f; angle += 5.0f) {
            float rad = angle * (M_PI / 180.0f); // Convert degree to radian
            float x = cx + radius * cos(rad);
            float y = cy + radius * sin(rad);
            glVertex2f(x, y);
        }
    glEnd();
}

// =========================================================================
// METHOD 3: Dynamic Trigonometric Loop (GL_TRIANGLE_FAN / GL_LINE_LOOP)
// Smooth rendering with adjustable segment count
// =========================================================================
void drawFilledCircle(float cx, float cy, float radius, int segments = 100) {
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy); // Center point
        for (int i = 0; i <= segments; ++i) {
            float theta = 2.0f * M_PI * float(i) / float(segments);
            float x = radius * cosf(theta);
            float y = radius * sinf(theta);
            glVertex2f(cx + x, cy + y);
        }
    glEnd();
}

void drawHollowCircle(float cx, float cy, float radius, float lineWidth = 2.0f, int segments = 100) {
    glLineWidth(lineWidth);
    glBegin(GL_LINE_LOOP);
        for (int i = 0; i < segments; ++i) {
            float theta = 2.0f * M_PI * float(i) / float(segments);
            float x = radius * cosf(theta);
            float y = radius * sinf(theta);
            glVertex2f(cx + x, cy + y);
        }
    glEnd();
}

// =========================================================================
// METHOD 4: Midpoint Circle Algorithm (Classic Computer Graphics Lab Method)
// Uses integer arithmetic and 8-way symmetry with GL_POINTS
// =========================================================================
void drawMidpointCircle(float cx, float cy, float radius) {
    glPointSize(3.0f);
    glBegin(GL_POINTS);
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow points

        int r = (int)(radius * 500); // Scale to screen space units for pixel loop
        int x = 0;
        int y = r;
        int p = 1 - r;

        auto plot8Points = [&](int xc, int yc, int x, int y) {
            float scale = 1.0f / 500.0f;
            glVertex2f(cx + x * scale, cy + y * scale);
            glVertex2f(cx - x * scale, cy + y * scale);
            glVertex2f(cx + x * scale, cy - y * scale);
            glVertex2f(cx - x * scale, cy - y * scale);
            glVertex2f(cx + y * scale, cy + x * scale);
            glVertex2f(cx - y * scale, cy + x * scale);
            glVertex2f(cx + y * scale, cy - x * scale);
            glVertex2f(cx - y * scale, cy - x * scale);
        };

        plot8Points(0, 0, x, y);

        while (x < y) {
            x++;
            if (p < 0) {
                p += 2 * x + 1;
            } else {
                y--;
                p += 2 * (x - y) + 1;
            }
            plot8Points(0, 0, x, y);
        }
    glEnd();
}

// =========================================================================
// METHOD 5: Other Shapes (Gradient, Ellipse, Arc, Flower)
// =========================================================================
void drawGradientCircle(float cx, float cy, float radius, int segments = 100) {
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(1.0f, 1.0f, 0.2f); // Yellow center
        glVertex2f(cx, cy);
        for (int i = 0; i <= segments; ++i) {
            glColor3f(0.8f, 0.2f, 0.8f); // Purple boundary
            float theta = 2.0f * M_PI * float(i) / float(segments);
            glVertex2f(cx + radius * cosf(theta), cy + radius * sinf(theta));
        }
    glEnd();
}

void drawConcentricCircles(float cx, float cy, float maxRadius, int numRings = 5) {
    float step = maxRadius / numRings;
    for (int i = numRings; i >= 1; --i) {
        float r = i * step;
        if (i % 2 == 0) {
            glColor3f(0.9f, 0.1f, 0.2f);
        } else {
            glColor3f(1.0f, 1.0f, 1.0f);
        }
        drawFilledCircle(cx, cy, r, 80);
    }
}

void drawEllipse(float cx, float cy, float rx, float ry, bool filled = true, int segments = 100) {
    if (filled) {
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(cx, cy);
            for (int i = 0; i <= segments; ++i) {
                float theta = 2.0f * M_PI * float(i) / float(segments);
                glVertex2f(cx + rx * cosf(theta), cy + ry * sinf(theta));
            }
        glEnd();
    } else {
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
            for (int i = 0; i < segments; ++i) {
                float theta = 2.0f * M_PI * float(i) / float(segments);
                glVertex2f(cx + rx * cosf(theta), cy + ry * sinf(theta));
            }
        glEnd();
    }
}

void drawArc(float cx, float cy, float radius, float startDeg, float endDeg, bool filled = true, int segments = 50) {
    float startRad = startDeg * (M_PI / 180.0f);
    float endRad = endDeg * (M_PI / 180.0f);
    float angleStep = (endRad - startRad) / segments;

    if (filled) {
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(cx, cy);
            for (int i = 0; i <= segments; ++i) {
                float theta = startRad + i * angleStep;
                glVertex2f(cx + radius * cosf(theta), cy + radius * sinf(theta));
            }
        glEnd();
    } else {
        glLineWidth(3.0f);
        glBegin(GL_LINE_STRIP);
            for (int i = 0; i <= segments; ++i) {
                float theta = startRad + i * angleStep;
                glVertex2f(cx + radius * cosf(theta), cy + radius * sinf(theta));
            }
        glEnd();
    }
}

// =========================================================================
// DISPLAY & INITIALIZATION
// =========================================================================

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // ---------------------------------------------------------------------
    // Top-Left: Direct Manual Vertices (Like cgm lab-3.1) & Simple Degree Loop
    // ---------------------------------------------------------------------
    // Method 1: Manual glVertex2f (Octagon circle)
    glPushMatrix();
    glTranslatef(-0.65f, 0.55f, 0.0f);
    glScalef(0.75f, 0.75f, 1.0f);
    drawDirectManualCircle();
    glPopMatrix();

    // Method 2: Simple Degree Loop (0 to 360)
    drawSimpleDegreeCircle(-0.25f, 0.55f, 0.22f);

    // ---------------------------------------------------------------------
    // Top-Right: Dynamic Trigonometric Circle & Midpoint Circle Algorithm
    // ---------------------------------------------------------------------
    glColor3f(0.2f, 0.8f, 0.4f);
    drawFilledCircle(0.25f, 0.55f, 0.22f);

    // Method 4: Midpoint Circle Algorithm
    drawMidpointCircle(0.65f, 0.55f, 0.22f);

    // ---------------------------------------------------------------------
    // Bottom-Left: Concentric Target Ring & Ellipse
    // ---------------------------------------------------------------------
    drawConcentricCircles(-0.65f, -0.45f, 0.30f, 5);

    glColor3f(0.6f, 0.4f, 0.9f);
    drawEllipse(-0.25f, -0.45f, 0.22f, 0.12f, true);

    // ---------------------------------------------------------------------
    // Bottom-Right: Gradient Circle & Pac-Man Arc
    // ---------------------------------------------------------------------
    drawGradientCircle(0.25f, -0.45f, 0.22f);

    glColor3f(1.0f, 0.9f, 0.1f);
    drawArc(0.65f, -0.45f, 0.25f, 30.0f, 330.0f, true);

    glFlush();
}

void init() {
    glClearColor(0.08f, 0.09f, 0.12f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowSize(800, 800);
    glutInitWindowPosition(150, 100);

    glutCreateWindow("Computer Graphics Lab - Circle Drawing Methods");

    init();

    glutDisplayFunc(display);

    glutMainLoop();

    return 0;
}
