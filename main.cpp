#ifndef GLOBALS_H
#define GLOBALS_H

#define PI           3.14159265f
#define STAR_COUNT   10000
#define DEBRIS_COUNT 800
#define COMET_COUNT  5
#define TAIL_LEN     40

/* ── Planet angles ── */
extern float angMercury;
extern float angVenus;
extern float angEarth;
extern float angMars;
extern float angJupiter;
extern float angSaturn;
extern float angUranus;
extern float angNeptune;
extern float angMoon;
extern float sunPulse;

/* ── Camera ── */
extern float camAz;
extern float camEl;
extern float camZoom;

/* ── Mouse ── */
extern int mouseBtn;
extern int mouseX;
extern int mouseY;

/* ── Stars ── */
extern float starX[STAR_COUNT];
extern float starY[STAR_COUNT];
extern float starZ[STAR_COUNT];
extern float starBright[STAR_COUNT];
extern int   starType[STAR_COUNT];

/* ── Asteroid Debris ── */
extern float debX[DEBRIS_COUNT];
extern float debY[DEBRIS_COUNT];
extern float debZ[DEBRIS_COUNT];
extern float debOrbR[DEBRIS_COUNT];
extern float debOrbAng[DEBRIS_COUNT];
extern float debSpeed[DEBRIS_COUNT];
extern float debSize[DEBRIS_COUNT];
extern float debR[DEBRIS_COUNT];
extern float debG[DEBRIS_COUNT];
extern float debB[DEBRIS_COUNT];

/* ── Comet struct ── */
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

extern Comet comets[COMET_COUNT];

/* ── Utility ── */
float rf(float lo, float hi);
void  getCamPos(float *cx, float *cy, float *cz);

#endif /* GLOBALS_H */
