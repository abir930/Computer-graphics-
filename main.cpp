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

//camera position using mouse
void onMouseMove(int x, int y)
{
    int dx = x - mouseX;
    int dy = y - mouseY;

    if (mouseBtn == GLUT_LEFT_BUTTON)
    {
        camAz += dx * 0.5f;
        camEl -= dy * 0.5f;

        if (camEl >  89) camEl =  89;
        if (camEl < -89) camEl = -89;
    }
    else if (mouseBtn == GLUT_RIGHT_BUTTON)
    {
        camZoom += dy * 0.15f;
        if (camZoom <  5) camZoom =  5;
        if (camZoom > 90) camZoom = 90;
    }

    mouseX = x;
    mouseY = y;
    glutPostRedisplay();
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

// Midpoint Circle algorithm
void MidpointCircle_Orbit(float radius, float r, float g, float b)
{
    int R   = (int)(radius * 50);
    float inv = 1.0f / 50.0f;

    int x = 0;
    int y = R;
    int d = 1 - R;

    glDisable(GL_LIGHTING);
    glColor3f(r, g, b);
    glPointSize(1.5f);

    #define PLOT8(px, py) \
        glVertex3f( (px)*inv, 0,  (py)*inv); \
        glVertex3f(-(px)*inv, 0,  (py)*inv); \
        glVertex3f( (px)*inv, 0, -(py)*inv); \
        glVertex3f(-(px)*inv, 0, -(py)*inv); \
        glVertex3f( (py)*inv, 0,  (px)*inv); \
        glVertex3f(-(py)*inv, 0,  (px)*inv); \
        glVertex3f( (py)*inv, 0, -(px)*inv); \
        glVertex3f(-(py)*inv, 0, -(px)*inv);

    glBegin(GL_POINTS);
    PLOT8(x, y)

    while (x < y)
    {
        x++;
        if (d < 0)
            d += 2 * x + 1;
        else
        {
            y--;
            d += 2 * (x - y) + 1;
        }
        PLOT8(x, y)
    }
    glEnd();

    #undef PLOT8
    glEnable(GL_LIGHTING);
}


void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float cx, cy, cz;
    getCamPos(&cx, &cy, &cz);
    gluLookAt(cx, cy, cz, 0, 0, 0, 0, 1, 0);

    drawStars();

    DDA_Line(-25, 0, 0,  25, 0, 0);
    DDA_Line( 0,  0, -25, 0, 0, 25);

    MidpointCircle_Orbit( 4,  0.4f, 0.4f, 0.4f);
    MidpointCircle_Orbit( 6,  0.5f, 0.4f, 0.3f);
    MidpointCircle_Orbit( 8,  0.2f, 0.5f, 0.8f);
    MidpointCircle_Orbit(10,  0.8f, 0.3f, 0.2f);
    MidpointCircle_Orbit(13,  0.8f, 0.6f, 0.3f);
    MidpointCircle_Orbit(16,  0.9f, 0.8f, 0.5f);
    MidpointCircle_Orbit(19,  0.4f, 0.9f, 0.9f);
    MidpointCircle_Orbit(22,  0.2f, 0.3f, 0.9f);
    MidpointCircle_Orbit(11.5f, 0.45f, 0.38f, 0.25f);

    Bresenham_Ticks( 8, 12);
    Bresenham_Ticks(13,  8);
    Bresenham_Ticks(16,  8);

    glPushMatrix();
    glDisable(GL_LIGHTING);
    float glow = 1.0f + 0.08f * sinf(sunPulse * PI / 180.0f);
    glColor3f(1.0f, 0.85f, 0.1f);
    glutSolidSphere(2.5f * glow, 50, 50);
    glColor3f(1.0f, 1.0f, 0.6f);
    glutSolidSphere(2.0f, 50, 50);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    float lpos[] = {0, 0, 0, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);

    drawPlanet( 4, angMercury, 0.55f, 0.55f, 0.55f, 0.28f);
    drawPlanet( 6, angVenus,   1.0f,  0.6f,  0.2f,  0.5f );
    drawPlanet(10, angMars,    0.9f,  0.3f,  0.15f, 0.4f );
    drawPlanet(13, angJupiter, 1.0f,  0.7f,  0.3f,  1.0f );
    drawPlanet(16, angSaturn,  1.0f,  0.85f, 0.5f,  0.85f);
    drawPlanet(19, angUranus,  0.4f,  0.9f,  0.9f,  0.7f );
    drawPlanet(22, angNeptune, 0.2f,  0.3f,  0.95f, 0.65f);
    drawSaturnRings(16, angSaturn);

    {
        float th = angEarth * PI / 180.0f;
        float c  = cosf(th);
        float s  = sinf(th);

        float M[16] = {
            c, 0, -s, 0,
            0, 1,  0, 0,
            s, 0,  c, 0,
            0, 0,  0, 1
        };

        glPushMatrix();
            glMultMatrixf(M);
            glTranslatef(8, 0, 0);
            glColor3f(0.15f, 0.45f, 1.0f);
            glutSolidSphere(0.6f, 30, 30);
            drawMoon(angMoon, 1.5f);
        glPopMatrix();
    }

    drawDebris();
    drawComets();
    drawHUD();

    glutSwapBuffers();
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
