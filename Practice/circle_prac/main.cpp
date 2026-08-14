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
// HELPER CIRCLE DRAWING FUNCTIONS
// =========================================================================

// 1. Draw Solid Filled Circle using GL_TRIANGLE_FAN
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

// 2. Draw Hollow Circle (Outline) using GL_LINE_LOOP
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

// 3. Draw Dotted Circle using GL_POINTS
void drawDottedCircle(float cx, float cy, float radius, float pointSize = 4.0f, int segments = 36) {
    glPointSize(pointSize);
    glBegin(GL_POINTS);
        for (int i = 0; i < segments; ++i) {
            float theta = 2.0f * M_PI * float(i) / float(segments);
            float x = radius * cosf(theta);
            float y = radius * sinf(theta);
            glVertex2f(cx + x, cy + y);
        }
    glEnd();
}

// 4. Draw Color Gradient Circle (Center to Boundary Transition)
void drawGradientCircle(float cx, float cy, float radius, int segments = 100) {
    glBegin(GL_TRIANGLE_FAN);
        // Center is bright yellow
        glColor3f(1.0f, 1.0f, 0.2f);
        glVertex2f(cx, cy);
        for (int i = 0; i <= segments; ++i) {
            // Radial vertex transitions to magenta/purple
            glColor3f(0.8f, 0.2f, 0.8f);
            float theta = 2.0f * M_PI * float(i) / float(segments);
            float x = radius * cosf(theta);
            float y = radius * sinf(theta);
            glVertex2f(cx + x, cy + y);
        }
    glEnd();
}

// 5. Draw Concentric Circles (Target Ring / Bullseye)
void drawConcentricCircles(float cx, float cy, float maxRadius, int numRings = 5) {
    float step = maxRadius / numRings;
    for (int i = numRings; i >= 1; --i) {
        float r = i * step;
        // Alternating colors
        if (i % 2 == 0) {
            glColor3f(0.9f, 0.1f, 0.2f); // Red
        } else {
            glColor3f(1.0f, 1.0f, 1.0f); // White
        }
        drawFilledCircle(cx, cy, r, 80);
    }
}

// 6. Draw Ellipse / Oval
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

// 7. Draw Arc / Semicircle (Sector)
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

// 8. Draw Flower / Rosette Circle Pattern
void drawCircleFlowerPattern(float cx, float cy, float r, int petals = 6) {
    // Draw outer petals
    for (int i = 0; i < petals; ++i) {
        float angle = 2.0f * M_PI * float(i) / float(petals);
        float px = cx + (r * 0.6f) * cosf(angle);
        float py = cy + (r * 0.6f) * sinf(angle);

        // Color cycle for petals
        glColor3f(0.2f + 0.1f * i, 0.7f, 1.0f - 0.1f * i);
        drawHollowCircle(px, py, r * 0.5f, 2.0f, 60);
    }
    // Draw central ring
    glColor3f(1.0f, 0.8f, 0.2f);
    drawHollowCircle(cx, cy, r * 0.6f, 3.0f, 60);
}

// =========================================================================
// DISPLAY & INITIALIZATION
// =========================================================================

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // ---------------------------------------------------------------------
    // Top-Left: Solid Circle & Hollow Circle
    // ---------------------------------------------------------------------
    glColor3f(0.2f, 0.8f, 0.4f); // Emerald Green
    drawFilledCircle(-0.65f, 0.55f, 0.22f);

    glColor3f(0.9f, 0.3f, 0.2f); // Coral Red Outline
    drawHollowCircle(-0.25f, 0.55f, 0.22f, 3.0f);

    // ---------------------------------------------------------------------
    // Top-Right: Dotted Circle & Gradient Sunburst
    // ---------------------------------------------------------------------
    glColor3f(0.1f, 0.7f, 1.0f); // Cyan Dots
    drawDottedCircle(0.25f, 0.55f, 0.22f, 5.0f, 32);

    drawGradientCircle(0.65f, 0.55f, 0.22f);

    // ---------------------------------------------------------------------
    // Bottom-Left: Concentric Target Ring & Ellipses
    // ---------------------------------------------------------------------
    drawConcentricCircles(-0.65f, -0.45f, 0.30f, 5);

    glColor3f(0.6f, 0.4f, 0.9f); // Purple Ellipse Filled
    drawEllipse(-0.25f, -0.35f, 0.22f, 0.12f, true);

    glColor3f(1.0f, 0.6f, 0.1f); // Orange Ellipse Wireframe
    drawEllipse(-0.25f, -0.65f, 0.12f, 0.22f, false);

    // ---------------------------------------------------------------------
    // Bottom-Right: Semicircle / Pacman Arc & Flower Pattern
    // ---------------------------------------------------------------------
    glColor3f(1.0f, 0.9f, 0.1f); // Yellow Pac-Man Arc
    drawArc(0.25f, -0.45f, 0.25f, 30.0f, 330.0f, true);

    drawCircleFlowerPattern(0.65f, -0.45f, 0.35f, 6);

    glFlush();
}

void init() {
    // Set background color to dark gray-blue
    glClearColor(0.08f, 0.09f, 0.12f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Set 2D Orthographic coordinate space [-1, 1] on both axes
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowSize(800, 800);
    glutInitWindowPosition(150, 100);

    glutCreateWindow("Computer Graphics Lab - Varied Circle Shapes");

    init();

    glutDisplayFunc(display);

    glutMainLoop();

    return 0;
}
