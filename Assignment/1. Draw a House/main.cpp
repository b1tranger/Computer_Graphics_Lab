#include <stdlib.h>

#include <cmath>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

// SKY

    glBegin(GL_QUADS);
    glColor3f(0.53f, 0.81f, 0.92f);
    glVertex2f(-1.0,-1.0);
    glVertex2f(1.0,-1.0);
    glVertex2f(1.0,1.0);
    glVertex2f(-1.0,1.0);
    glEnd();

    // SUN

    glPointSize(80);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 0.84f, 0.0f);
    glVertex2f(-0.8,0.8);
    glEnd();

    // SUN RAY

    glLineWidth(10);
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.65f, 0.0f);
    glVertex2f(-0.8,0.65);
    glVertex2f(-0.8,0.4);
    glEnd();

    glLineWidth(10);
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.65f, 0.0f);
    glVertex2f(-0.87,0.65);
    glVertex2f(-0.95,0.45);
    glEnd();

    glLineWidth(10);
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.65f, 0.0f);
    glVertex2f(-0.73,0.65);
    glVertex2f(-0.65,0.45);
    glEnd();

    glLineWidth(10);
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.65f, 0.0f);
    glVertex2f(-0.65,0.67);
    glVertex2f(-0.50,0.55);
    glEnd();

    glLineWidth(7);
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.65f, 0.0f);
    glVertex2f(-0.65,0.77);
    glVertex2f(-0.45,0.74);
    glEnd();

    // Ground

    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.39f, 0.0f);
    glVertex2f(-1.0,-1.0);
    glVertex2f(1.0,-1.0);
    glVertex2f(1.0,-0.5);
    glVertex2f(-1.0,-0.5);
    glEnd();

//Chimney

    glBegin(GL_QUADS);
    glColor3f(0.55f, 0.27f, 0.07f);
    glVertex2f(0.4,0.2);
    glVertex2f(0.5,0.2);
    glVertex2f(0.5,0.7);
    glVertex2f(0.4,0.7);
    glEnd();

    // Smoke

    glPointSize(8);
    glBegin(GL_POINTS);
    glColor3f(0.13f, 0.13f, 0.13f);
    glVertex2f(0.43,0.71);
    glEnd();

    glPointSize(8);
    glBegin(GL_POINTS);
    glColor3f(0.13f, 0.13f, 0.13f);
    glVertex2f(0.46,0.72);
    glEnd();

    glPointSize(8);
    glBegin(GL_POINTS);
    glColor3f(0.13f, 0.13f, 0.13f);
    glVertex2f(0.42,0.75);
    glEnd();

    glPointSize(8);
    glBegin(GL_POINTS);
    glColor3f(0.13f, 0.13f, 0.13f);
    glVertex2f(0.47,0.76);
    glEnd();

    glPointSize(8);
    glBegin(GL_POINTS);
    glColor3f(0.13f, 0.13f, 0.13f);
    glVertex2f(0.47,0.79);
    glEnd();

    glPointSize(10);
    glBegin(GL_POINTS);
    glColor3f(0.13f, 0.13f, 0.13f);
    glVertex2f(0.49,0.82);
    glEnd();

    glPointSize(10);
    glBegin(GL_POINTS);
    glColor3f(0.13f, 0.13f, 0.13f);
    glVertex2f(0.45,0.86);
    glEnd();

    glPointSize(15);
    glBegin(GL_POINTS);
    glColor3f(0.13f, 0.13f, 0.13f);
    glVertex2f(0.52,0.88);
    glEnd();

    glPointSize(15);
    glBegin(GL_POINTS);
    glColor3f(0.13f, 0.13f, 0.13f);
    glVertex2f(0.46,0.91);
    glEnd();

    glPointSize(20);
    glBegin(GL_POINTS);
    glColor3f(0.13f, 0.13f, 0.13f);
    glVertex2f(0.55,0.96);
    glEnd();

//    HOUSE ROOF
    glBegin(GL_TRIANGLES);
    glColor3f(0.24f, 0.15f, 0.14f);
    glVertex2f(-0.9,0.2);
    glVertex2f(0.0,0.7);
    glVertex2f(0.9,0.2);
    glEnd();

//HOUSE BODY
    glBegin(GL_QUADS);
    glColor3f(0.55f, 0.27f, 0.07f);
    glVertex2f(-0.6,-0.5);
    glVertex2f(0.6,-0.5);
    glVertex2f(0.6,0.2);
    glVertex2f(-0.6,0.2);
    glEnd();


//Window left | FULLY DRAWN LINE BORDERS
    glBegin(GL_QUADS);
    glColor3f(0.53f, 0.81f, 0.98f);
    glVertex2f(-0.5,-0.1);
    glVertex2f(-0.3,-0.1);
    glVertex2f(-0.3,0.1);
    glVertex2f(-0.5,0.1);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(0.0,0.0,0.0);
    glVertex2f(-0.5,-0.1);
    glVertex2f(-0.5,0.1);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(0.0,0.0,0.0);
    glVertex2f(-0.3,-0.1);
    glVertex2f(-0.3,0.1);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(0.0,0.0,0.0);
    glVertex2f(-0.5,-0.1);
    glVertex2f(-0.3,-0.1);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(0.0,0.0,0.0);
    glVertex2f(-0.5,0.1);
    glVertex2f(-0.3,0.1);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(0.0,0.0,0.0);
    glVertex2f(-0.4,-0.1);
    glVertex2f(-0.4,0.1);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(0.0,0.0,0.0);
    glVertex2f(-0.4,-0.1);
    glVertex2f(-0.4,0.1);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(0.0,0.0,0.0);
    glVertex2f(-0.5,0.0);
    glVertex2f(-0.3,0.0);
    glEnd();

//Window right | 4 smaller squares

    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.5,-0.1);
    glVertex2f(0.3,-0.1);
    glVertex2f(0.3,0.1);
    glVertex2f(0.5,0.1);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.53f, 0.81f, 0.98f);
    glVertex2f(0.49,-0.09);
    glVertex2f(0.41,-0.09);
    glVertex2f(0.41,-0.01);
    glVertex2f(0.49,-0.01);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.53f, 0.81f, 0.98f);
    glVertex2f(0.40,-0.09);
    glVertex2f(0.31,-0.09);
    glVertex2f(0.31,-0.01);
    glVertex2f(0.40,-0.01);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.53f, 0.81f, 0.98f);
    glVertex2f(0.49,0.0);
    glVertex2f(0.41,0.0);
    glVertex2f(0.41,0.09);
    glVertex2f(0.49,0.09);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.53f, 0.81f, 0.98f);
    glVertex2f(0.40,0.0);
    glVertex2f(0.31,0.0);
    glVertex2f(0.31,0.09);
    glVertex2f(0.40,0.09);
    glEnd();

//    DOOR

    glBegin(GL_QUADS);
    glColor3f(0.80f, 0.52f, 0.25f);
    glVertex2f(-0.2,-0.5);
    glVertex2f(0.2,-0.5);
    glVertex2f(0.2,0.0);
    glVertex2f(-0.2,0.0);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(0.0,0.0,0.0);
    glVertex2f(0.0,0.0);
    glVertex2f(0.0,-0.5);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(0.0,0.0,0.0);
    glVertex2f(-0.2,0.0);
    glVertex2f(-0.2,-0.5);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(0.0,0.0,0.0);
    glVertex2f(0.2,0.0);
    glVertex2f(0.2,-0.5);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(0.0,0.0,0.0);
    glVertex2f(-0.2,0.0);
    glVertex2f(0.2,0.0);
    glEnd();

    // DOOR HANDLE

    glLineWidth(50);
    glBegin(GL_LINES);
    glColor3f(0.0,0.0,0.0);
    glVertex2f(0.03,-0.2);
    glVertex2f(0.03,-0.3);
    glEnd();

    glLineWidth(50);
    glBegin(GL_LINES);
    glColor3f(0.0,0.0,0.0);
    glVertex2f(-0.03,-0.2);
    glVertex2f(-0.03,-0.3);
    glEnd();

    glFlush();
}

void init()
{
    glClearColor(0.0,0.0,0.0,1.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(-1.0,1.0,-1.0,1.0);
}

int main(int argc,char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowSize(700,700);
    glutInitWindowPosition(200,100);

    glutCreateWindow("Lab 2 - Basic 2D Shapes");

    init();

    glutDisplayFunc(display);

    glutMainLoop();

    return 0;
}
