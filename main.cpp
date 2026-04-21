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

// 2D transform
void drawMoon(float angleDeg, float dist)
{
    float th = angleDeg * PI / 180.0f;

    glPushMatrix();
        glTranslatef(dist * cosf(th), 0, dist * sinf(th));
        glColor3f(0.85f, 0.85f, 0.85f);
        glutSolidSphere(0.2, 20, 20);
    glPopMatrix();
}

// 3D rotation matrix
void drawPlanet(float dist, float angleDeg,
                float r, float g, float b, float size)
{
    float th = angleDeg * PI / 180.0f;
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
        glTranslatef(dist, 0, 0);
        glColor3f(r, g, b);
        glutSolidSphere(size, 30, 30);
    glPopMatrix();
}

//saturn ring
void drawSaturnRings(float dist, float angleDeg)
{
    float th = angleDeg * PI / 180.0f;
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
        glTranslatef(dist, 0, 0);

        glDisable(GL_LIGHTING);
        glColor4f(0.85f, 0.75f, 0.5f, 0.5f);
        glRotatef(20, 0, 0, 1);

        glBegin(GL_TRIANGLE_STRIP);
        int i;
        for (i = 0; i <= 60; i++)
        {
            float a = 2 * PI * i / 60;
            glVertex3f(1.7f * cosf(a), 0, 1.7f * sinf(a));
            glVertex3f(1.1f * cosf(a), 0, 1.1f * sinf(a));
        }
        glEnd();

        glEnable(GL_LIGHTING);
    glPopMatrix();
}

//random star color and position
void initStars()
{
    srand(42);

    int i;
    for (i = 0; i < STAR_COUNT; i++)
    {
        float az = rf(0, 2 * PI);
        float el = rf(-PI / 2.0f, PI / 2.0f);

        el = el * fabsf(el) * 0.5f + rf(-0.25f, 0.25f);

        float r = rf(120.0f, 160.0f);

        starX[i] = r * cosf(el) * cosf(az);
        starY[i] = r * sinf(el);
        starZ[i] = r * cosf(el) * sinf(az);

        starBright[i] = rf(0.2f, 1.0f);

        int t = rand() % 10;
        if (t < 5)       starType[i] = 0;
        else if (t < 7)  starType[i] = 1;
        else if (t < 9)  starType[i] = 2;
        else             starType[i] = 3;
    }
}


void drawStars()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    int i;

    glPointSize(1.0f);
    glBegin(GL_POINTS);
    for (i = 0; i < STAR_COUNT; i++)
    {
        float b = starBright[i];
        if (starType[i] == 0) glColor3f(b,        b,        b      );
        if (starType[i] == 1) glColor3f(b,        b*0.85f,  b*0.6f );
        if (starType[i] == 2) glColor3f(b*0.7f,   b*0.85f,  b      );
        if (starType[i] == 3) glColor3f(b,        b*0.3f,   b*0.2f );
        glVertex3f(starX[i], starY[i], starZ[i]);
    }
    glEnd();

    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (i = 0; i < STAR_COUNT; i += 4)
    {
        if (starBright[i] > 0.6f)
        {
            if (starType[i] == 0) glColor3f(1.0f, 1.0f,  1.0f );
            if (starType[i] == 1) glColor3f(1.0f, 0.92f, 0.6f );
            if (starType[i] == 2) glColor3f(0.7f, 0.85f, 1.0f );
            if (starType[i] == 3) glColor3f(1.0f, 0.35f, 0.2f );
            glVertex3f(starX[i], starY[i], starZ[i]);
        }
    }
    glEnd();

    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (i = 0; i < STAR_COUNT; i += 18)
    {
        if (starBright[i] > 0.85f)
        {
            if (starType[i] == 0) glColor3f(1.0f,  1.0f,   1.0f );
            if (starType[i] == 1) glColor3f(1.0f,  0.95f,  0.65f);
            if (starType[i] == 2) glColor3f(0.75f, 0.9f,   1.0f );
            if (starType[i] == 3) glColor3f(1.0f,  0.4f,   0.25f);
            glVertex3f(starX[i], starY[i], starZ[i]);
        }
    }
    glEnd();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}
// kabbo

//asteroid orbital position and color
void initDebris()
{
    int i;
    for (i = 0; i < DEBRIS_COUNT; i++)
    {
        debOrbR[i]   = rf(10.5f, 12.5f);
        debOrbAng[i] = rf(0.0f, 360.0f);
        debSpeed[i]  = rf(0.05f, 0.25f);
        debSize[i]   = rf(1.0f, 2.8f);

        float base = rf(0.35f, 0.65f);
        debR[i] = base + rf(-0.1f,  0.15f);
        debG[i] = base + rf(-0.05f, 0.05f);
        debB[i] = base * rf(0.6f,   0.85f);

        float th = debOrbAng[i] * PI / 180.0f;
        debX[i] = debOrbR[i] * cosf(th);
        debY[i] = rf(-0.3f, 0.3f);
        debZ[i] = debOrbR[i] * sinf(th);
    }
}


//asteroid belt position update
void updateDebris()
{
    int i;
    for (i = 0; i < DEBRIS_COUNT; i++)
    {
        debOrbAng[i] += debSpeed[i];
        if (debOrbAng[i] > 360.0f) debOrbAng[i] -= 360.0f;

        float th = debOrbAng[i] * PI / 180.0f;
        debX[i] = debOrbR[i] * cosf(th);
        debZ[i] = debOrbR[i] * sinf(th);
    }
}


//orbital of asteroid belt
void drawDebris()
{
    glDisable(GL_LIGHTING);

    int i;
    for (i = 0; i < DEBRIS_COUNT; i++)
    {
        glPointSize(debSize[i]);
        glColor3f(debR[i], debG[i], debB[i]);
        glBegin(GL_POINTS);
        glVertex3f(debX[i], debY[i], debZ[i]);
        glEnd();
    }

    glEnable(GL_LIGHTING);
}


//comets orbital and tail buffer
void initComets()
{
    int i;
    for (i = 0; i < COMET_COUNT; i++)
    {
        comets[i].a        = rf(18.0f, 32.0f);
        comets[i].b        = rf(5.0f,  12.0f);
        comets[i].incline  = rf(-40.0f, 40.0f);
        comets[i].angSpeed = rf(0.15f,  0.4f);
        comets[i].ang      = rf(0.0f,  360.0f);
        comets[i].tailHead = 0;

        comets[i].cr = 1.0f;
        comets[i].cg = 0.0f;
        comets[i].cb = 0.0f;

        float th = comets[i].ang * PI / 180.0f;
        float hx = comets[i].a * cosf(th);
        float hz = comets[i].b * sinf(th);

        int j;
        for (j = 0; j < TAIL_LEN; j++)
        {
            comets[i].tailX[j] = hx;
            comets[i].tailY[j] = 0;
            comets[i].tailZ[j] = hz;
        }
    }
}


//comets 3D position using ellipse and incline formula
void cometPos(Comet *c, float ang, float *ox, float *oy, float *oz)
{
    float th  = ang * PI / 180.0f;
    float lx  = c->a * cosf(th);
    float lz  = c->b * sinf(th);
    float inc = c->incline * PI / 180.0f;

    *ox = lx;
    *oy = lz * sinf(inc);
    *oz = lz * cosf(inc);
}


//comets position update in every frame
void updateComets()
{
    int i;
    for (i = 0; i < COMET_COUNT; i++)
    {
        comets[i].ang += comets[i].angSpeed;
        if (comets[i].ang > 360) comets[i].ang -= 360;

        float hx, hy, hz;
        cometPos(&comets[i], comets[i].ang, &hx, &hy, &hz);

        int h = comets[i].tailHead;
        comets[i].tailX[h] = hx;
        comets[i].tailY[h] = hy;
        comets[i].tailZ[h] = hz;
        comets[i].tailHead = (h + 1) % TAIL_LEN;
    }
}


//comets tail and dust particle
void drawComets()
{
    glDisable(GL_LIGHTING);

    int i;
    for (i = 0; i < COMET_COUNT; i++)
    {
        Comet *c = &comets[i];

        float hx, hy, hz;
        cometPos(c, c->ang, &hx, &hy, &hz);

        glBegin(GL_LINE_STRIP);
        int j;
        for (j = 0; j < TAIL_LEN; j++)
        {
            int   idx   = (c->tailHead + j) % TAIL_LEN;
            float alpha = (float)j / TAIL_LEN;
            glColor4f(c->cr, c->cg, c->cb, alpha * alpha * 0.8f);
            glVertex3f(c->tailX[idx], c->tailY[idx], c->tailZ[idx]);
        }
        glEnd();

        glPointSize(1.5f);
        glBegin(GL_POINTS);
        for (j = TAIL_LEN / 2; j < TAIL_LEN; j++)
        {
            int   idx   = (c->tailHead + j) % TAIL_LEN;
            float alpha = (float)(j - TAIL_LEN / 2) / (TAIL_LEN / 2);
            glColor4f(c->cr, c->cg, c->cb, alpha * 0.5f);

            float sc = 0.08f * (1.0f - alpha);
            glVertex3f(c->tailX[idx] + rf(-sc, sc),
                       c->tailY[idx] + rf(-sc, sc),
                       c->tailZ[idx] + rf(-sc, sc));
        }
        glEnd();

        glPointSize(5.0f);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_POINTS);
        glVertex3f(hx, hy, hz);
        glEnd();

        glPointSize(3.0f);
        glColor4f(c->cr, c->cg, c->cb, 0.9f);
        glBegin(GL_POINTS);
        glVertex3f(hx, hy, hz);
        glEnd();
    }

    glEnable(GL_LIGHTING);
}


//camera info at the screen
void drawHUD()
{
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glColor4f(0, 0, 0, 0.6f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glVertex2i(0, 0);  glVertex2i(w, 0);
    glVertex2i(w, 52); glVertex2i(0, 52);
    glEnd();

    const char *hint =
        "MOUSE: [Left-Drag] Rotate  |  [Right-Drag] Zoom  |  [Middle] Reset Camera";
    glColor3f(0.9f, 0.9f, 0.5f);
    glRasterPos2i(10, 34);
    const char *p;
    for (p = hint; *p; p++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *p);

    char info[128];
    snprintf(info, sizeof(info),
             "Azimuth: %6.1f  Elevation: %6.1f  Zoom: %5.1f",
             camAz, camEl, camZoom);
    glColor3f(0.6f, 0.9f, 1.0f);
    glRasterPos2i(10, 12);
    for (p = info; *p; p++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *p);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}


//update every frame
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




























