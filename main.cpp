#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI           3.14159265f
#define STAR_COUNT   10000
#define DEBRIS_COUNT 800
#define COMET_COUNT  5
#define TAIL_LEN     40

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

typedef struct {
    float a;
    float b;
    float incline;
    float angSpeed;
    float ang;
    float tailX[TAIL_LEN];
    float tailY[TAIL_LEN];
    float tailZ[TAIL_LEN];
    int   tailHead;
    float cr;
    float cg;
    float cb;
} Comet;

Comet comets[COMET_COUNT];


float rf(float lo, float hi)
{
    return lo + ((float)rand() / RAND_MAX) * (hi - lo);
}


void getCamPos(float *cx, float *cy, float *cz)
{
    float az = camAz * PI / 180.0f;
    float el = camEl * PI / 180.0f;

    *cx = camZoom * cosf(el) * sinf(az);
    *cy = camZoom * sinf(el);
    *cz = camZoom * cosf(el) * cosf(az);
}


void onMouseBtn(int btn, int state, int x, int y)
{
    if (state == GLUT_DOWN)
    {
        mouseBtn = btn;
        mouseX   = x;
        mouseY   = y;

        if (btn == GLUT_MIDDLE_BUTTON)
        {
            camAz   = 30;
            camEl   = 25;
            camZoom = 38;
            glutPostRedisplay();
        }
    }
    else
    {
        mouseBtn = -1;
    }
}


// DDA algorithm
void DDA_Line(float x1, float y1, float z1,
              float x2, float y2, float z2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float dz = z2 - z1;

    float steps = fabsf(dx);
    if (fabsf(dy) > steps) steps = fabsf(dy);
    if (fabsf(dz) > steps) steps = fabsf(dz);
    if (steps == 0) return;

    float xi = dx / steps;
    float yi = dy / steps;
    float zi = dz / steps;

    float x = x1;
    float y = y1;
    float z = z1;

    glDisable(GL_LIGHTING);
    glColor4f(0.3f, 0.3f, 0.3f, 0.5f);
    glPointSize(1.0f);

    glBegin(GL_POINTS);
    int i;
    for (i = 0; i <= (int)steps; i++)
    {
        glVertex3f(x, y, z);
        x += xi;
        y += yi;
        z += zi;
    }
    glEnd();

    glEnable(GL_LIGHTING);
}

// Bresenham algorithm
void Bresenham_Ticks(float radius, int numTicks)
{
    glDisable(GL_LIGHTING);
    glColor3f(0.4f, 0.4f, 0.4f);

    int i;
    for (i = 0; i < numTicks; i++)
    {
        float theta = 2.0f * PI * i / numTicks;

        int sc = 100;

        int x0 = (int)((radius - 0.15f) * cosf(theta) * sc);
        int y0 = (int)((radius - 0.15f) * sinf(theta) * sc);
        int x1 = (int)((radius + 0.15f) * cosf(theta) * sc);
        int y1 = (int)((radius + 0.15f) * sinf(theta) * sc);

        int ddx = abs(x1 - x0);
        int ddy = -abs(y1 - y0);
        int sx  = (x0 < x1) ? 1 : -1;
        int sy  = (y0 < y1) ? 1 : -1;
        int err = ddx + ddy;
        int e2;

        glBegin(GL_POINTS);
        while (1)
        {
            glVertex3f((float)x0 / sc, 0, (float)y0 / sc);
            if (x0 == x1 && y0 == y1) break;

            e2 = 2 * err;
            if (e2 >= ddy) { err += ddy; x0 += sx; }
            if (e2 <= ddx) { err += ddx; y0 += sy; }
        }
        glEnd();
    }

    glEnable(GL_LIGHTING);
}



//openGL initialization
void init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float amb[] = {0.1f, 0.1f, 0.1f, 1};
    float dif[] = {1, 1, 1, 1};
    float lp[]  = {0, 0, 0, 1};
    glLightfv(GL_LIGHT0, GL_AMBIENT,  amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  dif);
    glLightfv(GL_LIGHT0, GL_POSITION, lp);

    glClearColor(0, 0, 0.03f, 1);

    initStars();
    initDebris();
    initComets();

    printf("\n=== Solar System ===\n");
    printf("Left-Drag: Rotate | Right-Drag: Zoom | Middle: Reset\n");
    printf("Stars: %d | Debris: %d | Comets: %d\n",
           STAR_COUNT, DEBRIS_COUNT, COMET_COUNT);
    printf("Algorithms: DDA | Bresenham | Midpoint Circle | 2D/3D Transform\n\n");
}


//angle update of every planet in 16ms
void update(int v)
{
    angMercury += 2.0f;  if (angMercury > 360) angMercury -= 360;
    angVenus   += 1.5f;  if (angVenus   > 360) angVenus   -= 360;
    angEarth   += 1.0f;  if (angEarth   > 360) angEarth   -= 360;
    angMars    += 0.8f;  if (angMars    > 360) angMars    -= 360;
    angJupiter += 0.4f;  if (angJupiter > 360) angJupiter -= 360;
    angSaturn  += 0.3f;  if (angSaturn  > 360) angSaturn  -= 360;
    angUranus  += 0.2f;  if (angUranus  > 360) angUranus  -= 360;
    angNeptune += 0.1f;  if (angNeptune > 360) angNeptune -= 360;
    angMoon    += 3.0f;  if (angMoon    > 360) angMoon    -= 360;
    sunPulse   += 1.5f;  if (sunPulse   > 360) sunPulse   -= 360;

    updateDebris();
    updateComets();

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}


//projection matrix update in window resize
void reshape(int w, int h)
{
    if (!h) h = 1;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float)w / h, 1, 300);

    glMatrixMode(GL_MODELVIEW);
}

//main function
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1100, 750);
    glutCreateWindow(
        "Solar System -- Stars | Debris | Comets | "
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
