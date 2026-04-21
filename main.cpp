
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "globals.h"
#include "algorithms.h"
#include "planets.h"
#include "animation.h"
#include "stars.h"
#include "comets.h"
#include "hud.h"


float angMercury = 0;
float angVenus   = 0;
float angEarth   = 0;
float angMars    = 0;
float angJupiter = 0;
float angSaturn  = 0;
float angUranus  = 0;
float angNeptune = 0;
float angMoon    = 0;
float sunPulse   = 0;

float camAz   = 30;
float camEl   = 25;
float camZoom = 38;

int mouseBtn = -1;
int mouseX   = 0;
int mouseY   = 0;

float starX[STAR_COUNT];
float starY[STAR_COUNT];
float starZ[STAR_COUNT];
float starBright[STAR_COUNT];
int   starType[STAR_COUNT];

float debX[DEBRIS_COUNT];
float debY[DEBRIS_COUNT];
float debZ[DEBRIS_COUNT];
float debOrbR[DEBRIS_COUNT];
float debOrbAng[DEBRIS_COUNT];
float debSpeed[DEBRIS_COUNT];
float debSize[DEBRIS_COUNT];
float debR[DEBRIS_COUNT];
float debG[DEBRIS_COUNT];
float debB[DEBRIS_COUNT];

Comet comets[COMET_COUNT];


float rf(float lo, float hi)
{
    return lo + ((float)rand() / RAND_MAX) * (hi - lo);
}


void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    /* Position camera */
    float cx, cy, cz;
    getCamPos(&cx, &cy, &cz);
    gluLookAt(cx, cy, cz, 0, 0, 0, 0, 1, 0);

    /* ── Shykha: background stars ── */
    drawStars();

    /* ── Abir: reference axes + orbit rings ── */
    drawAxes();
    drawAllOrbits();

    /* ── Kabbo: sun + all planets ── */
    drawSun();
    drawAllPlanets();

    /* ── Shykha: asteroid belt + comets ── */
    drawDebris();
    drawComets();

    /* ── Shykha: HUD overlay ── */
    drawHUD();

    glutSwapBuffers();
}


void init(void)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float amb[] = {0.1f, 0.1f, 0.1f, 1.0f};
    float dif[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float lp[]  = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT,  amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  dif);
    glLightfv(GL_LIGHT0, GL_POSITION, lp);

    glClearColor(0.0f, 0.0f, 0.03f, 1.0f);

    /* Initialise subsystems */
    initStars();
    initDebris();
    initComets();

    printf("\n=== Solar System Simulation ===\n");
    printf("Team: Abir | Kabbo | Shykha\n");
    printf("Left-Drag: Rotate | Right-Drag: Zoom | Middle: Reset\n");
    printf("Stars: %d  |  Debris: %d  |  Comets: %d\n",
           STAR_COUNT, DEBRIS_COUNT, COMET_COUNT);
    printf("Algorithms: DDA | Bresenham | Midpoint Circle | 2D/3D Transform\n\n");
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1100, 750);
    glutCreateWindow(
        "Solar System -- Abir | Kabbo | Shykha  --  "
        "DDA | Bresenham | Midpoint | 2D/3D Transform");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(onMouseBtn);
    glutMotionFunc(onMouseMove);
    glutTimerFunc(0, update, 0);

    glutMainLoop();
    return 0;
}
