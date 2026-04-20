#include <GL/glut.h>
#include <math.h>

//Constants
#define PI 3.1416

// Global Variables
float earthAngle = 0.0f;
float moonAngle  = 0.0f;

//Initialization
void init()
{
    glClearColor(0.0, 0.0, 0.05, 1.0); // dark space background
    glEnable(GL_DEPTH_TEST);
}

//Orbit Drawing
void drawOrbit(float radius)
{
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_POINTS);
    for (int i = 0; i < 360; i++)
    {
        float rad = i * PI / 180.0;
        glVertex3f(radius * cos(rad), 0, radius * sin(rad));
    }
    glEnd();
}

//Draw Planet
void drawPlanet(float distance, float angle, float r, float g, float b, float size)
{
    glPushMatrix();
        glRotatef(angle, 0, 1, 0);     // revolution
        glTranslatef(distance, 0, 0);  // distance from sun
        glColor3f(r, g, b);
        glutSolidSphere(size, 30, 30);
    glPopMatrix();
}

//Draw Moon
void drawMoon(float distance, float angle)
{
    glPushMatrix();
        glRotatef(angle, 0, 1, 0);
        glTranslatef(distance, 0, 0);
        glColor3f(0.8, 0.8, 0.8);
        glutSolidSphere(0.2, 20, 20);
    glPopMatrix();
}

//Scene Rendering
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Camera setup
    gluLookAt(0, 6, 12,
              0, 0, 0,
              0, 1, 0);

    //Sun
    glColor3f(1.0, 0.8, 0.0);
    glutSolidSphere(1.5, 40, 40);

    //Orbits
    drawOrbit(4.0);
    drawOrbit(7.0);

    //Earth
    glPushMatrix();
        glRotatef(earthAngle, 0, 1, 0);
        glTranslatef(4.0, 0, 0);

        glColor3f(0.2, 0.5, 1.0);
        glutSolidSphere(0.5, 30, 30);

        // Moon orbit
        drawOrbit(1.2);
        drawMoon(1.2, moonAngle);

    glPopMatrix();

    //Another Planet
    drawPlanet(7.0, earthAngle * 0.5, 1.0, 0.4, 0.2, 0.6);

    glutSwapBuffers();
}

// Animation
void update(int value)
{
    earthAngle += 1.0f;
    moonAngle  += 3.0f;

    if (earthAngle > 360) earthAngle -= 360;
    if (moonAngle  > 360) moonAngle  -= 360;

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// Window Resize
void reshape(int w, int h)
{
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float)w / h, 1, 100);

    glMatrixMode(GL_MODELVIEW);
}

// Main
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(900, 650);
    glutCreateWindow("Solar System - Initial Version");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, update, 0);

    glutMainLoop();
    return 0;
}
