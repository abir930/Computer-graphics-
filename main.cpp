#include <GL/glut.h>
#include <math.h>

#define PI 3.1416

// Global Variables
float earthAngle = 0.0f;
float moonAngle  = 0.0f;

// Initialization
void init()
{
    glClearColor(0.0, 0.0, 0.05, 1.0);
    glEnable(GL_DEPTH_TEST);
}

// DDA Line Algorithm
void DDA_Line(float x1, float y1, float z1,
              float x2, float y2, float z2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float dz = z2 - z1;

    float steps = fabs(dx);
    if (fabs(dy) > steps) steps = fabs(dy);
    if (fabs(dz) > steps) steps = fabs(dz);

    float xi = dx / steps;
    float yi = dy / steps;
    float zi = dz / steps;

    float x = x1;
    float y = y1;
    float z = z1;

    glBegin(GL_POINTS);
    for (int i = 0; i <= steps; i++)
    {
        glVertex3f(x, y, z);
        x += xi;
        y += yi;
        z += zi;
    }
    glEnd();
}

//  Orbit using DDA
void drawOrbitDDA(float radius)
{
    glColor3f(0.6, 0.6, 0.6);

    float prevX = radius;
    float prevZ = 0;

    for (int i = 1; i <= 360; i++)
    {
        float angle = i * PI / 180.0;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        DDA_Line(prevX, 0, prevZ, x, 0, z);

        prevX = x;
        prevZ = z;
    }
}

//  Draw Planet
void drawPlanet(float distance, float angle, float r, float g, float b, float size)
{
    glPushMatrix();
        glRotatef(angle, 0, 1, 0);
        glTranslatef(distance, 0, 0);
        glColor3f(r, g, b);
        glutSolidSphere(size, 30, 30);
    glPopMatrix();
}

// Draw Moon
void drawMoon(float distance, float angle)
{
    glPushMatrix();
        glRotatef(angle, 0, 1, 0);
        glTranslatef(distance, 0, 0);
        glColor3f(0.8, 0.8, 0.8);
        glutSolidSphere(0.2, 20, 20);
    glPopMatrix();
}

// Scene
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(0, 6, 12,
              0, 0, 0,
              0, 1, 0);

    //  Sun
    glColor3f(1.0, 0.8, 0.0);
    glutSolidSphere(1.5, 40, 40);

    //Orbits using DDA
    drawOrbitDDA(4.0);
    drawOrbitDDA(7.0);

    //Earth + Moon
    glPushMatrix();
        glRotatef(earthAngle, 0, 1, 0);
        glTranslatef(4.0, 0, 0);

        glColor3f(0.2, 0.5, 1.0);
        glutSolidSphere(0.5, 30, 30);

        drawOrbitDDA(1.2);
        drawMoon(1.2, moonAngle);

    glPopMatrix();

    // Another Planet
    drawPlanet(7.0, earthAngle * 0.5, 1.0, 0.4, 0.2, 0.6);

    glutSwapBuffers();
}

//Animation
void update(int value)
{
    earthAngle += 1.0f;
    moonAngle  += 3.0f;

    if (earthAngle > 360) earthAngle -= 360;
    if (moonAngle  > 360) moonAngle  -= 360;

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// Reshape
void reshape(int w, int h)
{
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float)w / h, 1, 100);

    glMatrixMode(GL_MODELVIEW);
}

// -------------------- Main --------------------
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(900, 650);
    glutCreateWindow("Solar System - DDA Version");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, update, 0);

    glutMainLoop();
    return 0;
}
