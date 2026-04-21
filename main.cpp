

#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include "globals.h"
#include "hud.h"


static void drawString3D(float wx, float wy, float wz, const char *str)
{
    glRasterPos3f(wx, wy, wz);
    const char *p;
    for (p = str; *p; p++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *p);
}

static void drawPlanetLabels(void)
{
    typedef struct { float dist; float ang; const char *name; } PlanetInfo;

    PlanetInfo planets[] = {
        {  4, angMercury, "Mercury" },
        {  6, angVenus,   "Venus"   },
        {  8, angEarth,   "Earth"   },
        { 10, angMars,    "Mars"    },
        { 13, angJupiter, "Jupiter" },
        { 16, angSaturn,  "Saturn"  },
        { 19, angUranus,  "Uranus"  },
        { 22, angNeptune, "Neptune" }
    };

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glColor3f(0.85f, 0.85f, 0.85f);

    int i;
    for (i = 0; i < 8; i++)
    {
        float th = planets[i].ang * PI / 180.0f;
        float x  = planets[i].dist * cosf(th);
        float z  = planets[i].dist * sinf(th);
        drawString3D(x + 0.2f, 0.5f, z + 0.2f, planets[i].name);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}



void drawHUD(void)
{
    /* Draw planet labels in 3D space first */
    drawPlanetLabels();

    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    /* Switch to 2D orthographic */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    /* Semi-transparent black bar */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0, 0, 0, 0.6f);
    glBegin(GL_QUADS);
    glVertex2i(0, 0);  glVertex2i(w, 0);
    glVertex2i(w, 52); glVertex2i(0, 52);
    glEnd();

    /* Hint line */
    const char *hint =
        "MOUSE: [Left-Drag] Rotate  |  [Right-Drag] Zoom  |  [Middle] Reset Camera";
    glColor3f(0.9f, 0.9f, 0.5f);
    glRasterPos2i(10, 34);
    const char *p;
    for (p = hint; *p; p++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *p);

    /* Camera info line */
    char info[128];
    snprintf(info, sizeof(info),
             "Azimuth: %6.1f  Elevation: %6.1f  Zoom: %5.1f",
             camAz, camEl, camZoom);
    glColor3f(0.6f, 0.9f, 1.0f);
    glRasterPos2i(10, 12);
    for (p = info; *p; p++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *p);

    /* Restore 3D state */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
