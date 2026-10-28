/*
 * GLUT Shapes Demo
 *
 * Written by Nigel Stewart November 2003
 *
 * This program is test harness for the sphere, cone
 * and torus shapes in GLUT.
 *
 * Spinning wireframe and smooth shaded shapes are
 * displayed until the ESC or q key is pressed.  The
 * number of geometry stacks and slices can be adjusted
 * using the + and - keys.
 */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include<stdio.h>



float Txval = 0, Tyval = 0,sval =1;
bool flagScale = false;

void triangle()
{
    glBegin(GL_QUADS);
    glColor3f(1.0,1.0,1.0);
    glVertex2f(2.0,2.0);
    glColor3f(0.0,1.0,0.0);
    glVertex2f(2.0,0.0);
    glColor3f(0.0,1.0,0.0);
    glVertex2f(0.0,0.0);
    glColor3f(0.0,1.0,0.0);
    glVertex2f(0.0,2.0);
    glEnd();
}

void display(void)
{

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D(-3, 3, -3, 3);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glViewport(0, 0 ,400 ,400);
    glPushMatrix();
    glTranslatef(Txval,Tyval,0);
    glRotatef(0,0,0,1);
    glScalef(sval,sval,1);
    triangle();
    glPopMatrix();
    glFlush();
    glutSwapBuffers();
}

void myKeyboardFunc( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 'r':
        printf("Hello\n");
        Txval+=0.2;
        break;
    case 'l':
        Txval-=0.2;
        break;
    case 'u':
        Tyval+=0.2;
        break;
    case 'd':
        Tyval-=0.2;
        break;
    case 'S':
        flagScale=true;
        break;
    case 's':
        flagScale=false;
        break;
    case 27: // Escape key
        exit(1);
    }
    glutPostRedisplay();
}

void animate()
{
    if (flagScale == true)
    {
        sval+= 0.005;
        if(sval > 3)
            sval = 0.005;
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    printf("key %d\n", key);
}




/* Program entry point */

int main (int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB
    | GLUT_DEPTH);
    glutInitWindowPosition(300,300);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Traingle-Demo");
    glShadeModel( GL_SMOOTH );
    glEnable( GL_DEPTH_TEST );
    glutDisplayFunc(display);
    glutKeyboardFunc(myKeyboardFunc);
    glutMainLoop();

    return 0;
}
