#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>

void identity(GLfloat *out)
{
    out[0] = 1.0f;
    out[1] = 0.0f;
    out[2] = 0.0f;
    out[3] = 0.0f;
    out[4] = 0.0f;
    out[5] = 1.0f;
    out[6] = 0.0f;
    out[7] = 0.0f;
    out[8] = 0.0f;
    out[9] = 0.0f;
    out[10] = 1.0f;
    out[11] = 0.0f;
    out[12] = 0.0f;
    out[13] = 0.0f;
    out[14] = 0.0f;
    out[15] = 1.0f;
}

void translate(GLfloat *out, GLfloat *in, GLfloat *v)
{
    for (int i = 0; i <= 11; i++)
    {
        out[i] = in[i];
    }
    out[12] = in[0] * v[0] + in[4] * v[1] + in[8] * v[2] + in[12];
    out[13] = in[1] * v[0] + in[5] * v[1] + in[9] * v[2] + in[13];
    out[14] = in[2] * v[0] + in[6] * v[1] + in[10] * v[2] + in[14];
    out[15] = in[3] * v[0] + in[7] * v[1] + in[11] * v[2] + in[15];
}

void scale(GLfloat *out, GLfloat *in, GLfloat *v)
{
    for (int i = 0; i < 16; i++)
    {
        out[i] = in[i];
    }
    out[0] *= v[0];
    out[1] *= v[0];
    out[2] *= v[0];
    out[3] *= v[0];
    out[4] *= v[1];
    out[5] *= v[1];
    out[6] *= v[1];
    out[7] *= v[1];
    out[8] *= v[2];
    out[9] *= v[2];
    out[10] *= v[2];
    out[11] *= v[2];
}

void rotateZ(GLfloat *out, GLfloat *in, float angle)
{
    // Nur die ersten beiden Spalten (Index 0..7) rotieren; restliche Elemente kopieren
    // 1) Originalwerte der ersten beiden Spalten sichern
    GLfloat t0 = in[0], t1 = in[1], t2 = in[2], t3 = in[3];
    GLfloat t4 = in[4], t5 = in[5], t6 = in[6], t7 = in[7];

    // 2) Rest der Matrix (Index 8..15) direkt übernehmen
    for (int i = 8; i < 16; i++)
    {
        out[i] = in[i];
    }

    // 3) Sinus und Cosinus berechnen
    float c = cosf(angle);
    float s = sinf(angle);

    // 4) Rotierte erste Spalte (Spalte 0)
    out[0] = t0 * c + t4 * s;
    out[1] = t1 * c + t5 * s;
    out[2] = t2 * c + t6 * s;
    out[3] = t3 * c + t7 * s;

    // 5) Rotierte zweite Spalte (Spalte 1)
    out[4] = t0 * -s + t4 * c;
    out[5] = t1 * -s + t5 * c;
    out[6] = t2 * -s + t6 * c;
    out[7] = t3 * -s + t7 * c;
}

void rotateX(GLfloat *out, GLfloat *in, float angle){
    // Spalte 1 (Y-Achse) und Spalte 2 (Z-Achse) rotieren
    // Werte der Spalten sichern
    GLfloat y0 = in[4], y1 = in[5], y2 = in[6], y3 = in[7];
    GLfloat z0 = in[8], z1 = in[9], z2 = in[10], z3 = in[11];

    // übrige Elemente (Spalte 0 und 3) kopieren
    out[0] = in[0]; out[1] = in[1]; out[2] = in[2]; out[3] = in[3];
    for (int i = 12; i < 16; ++i) out[i] = in[i];

    float c = cosf(angle);
    float s = sinf(angle);

    // rotiere Spalte Y -> neue Spalte Y
    out[4] = y0 * c + z0 * s;
    out[5] = y1 * c + z1 * s;
    out[6] = y2 * c + z2 * s;
    out[7] = y3 * c + z3 * s;

    // rotiere Spalte Z -> neue Spalte Z
    out[8]  = y0 * -s + z0 * c;
    out[9]  = y1 * -s + z1 * c;
    out[10] = y2 * -s + z2 * c;
    out[11] = y3 * -s + z3 * c;
}

void rotateY(GLfloat *out, GLfloat *in, float angle){
    // Spalte 0 (X-Achse) und Spalte 2 (Z-Achse) rotieren
    GLfloat x0 = in[0], x1 = in[1], x2 = in[2], x3 = in[3];
    GLfloat z0 = in[8], z1 = in[9], z2 = in[10], z3 = in[11];

    // übrige Elemente (Spalte 1 und 3) kopieren
    for (int i = 4; i < 8; ++i) out[i] = in[i];
    out[12] = in[12]; out[13] = in[13]; out[14] = in[14]; out[15] = in[15];

    float c = cosf(angle);
    float s = sinf(angle);

    // rotiere Spalte X -> neue Spalte X
    out[0] = x0 * c + z0 * -s;
    out[1] = x1 * c + z1 * -s;
    out[2] = x2 * c + z2 * -s;
    out[3] = x3 * c + z3 * -s;

    // rotiere Spalte Z -> neue Spalte Z
    out[8]  = x0 * s + z0 * c;
    out[9]  = x1 * s + z1 * c;
    out[10] = x2 * s + z2 * c;
    out[11] = x3 * s + z3 * c;
}


void normalize(GLfloat *in, GLfloat *out)
{
    float length = sqrt(in[0] * in[0] + in[1] * in[1] + in[2] * in[2]);
    out[0] = in[0] / length;
    out[1] = in[1] / length;
    out[2] = in[2] / length;
}

void cross(GLfloat *out, GLfloat *a, GLfloat *b)
{
    out[0] = a[1] * b[2] - a[2] * b[1];
    out[1] = a[2] * b[0] - a[0] * b[2];
    out[2] = a[0] * b[1] - a[1] * b[0];
}

// TODO
void lookAt(GLfloat *out, GLfloat *eye, GLfloat *center, GLfloat *up)
{
    // forward vector center - eye
    GLfloat n[3] = {eye[0] - center[0], eye[1] - center[1], eye[2] - center[2]}; // ← !!

    // normalize forward vecotor bekommt die länge 1
    normalize(n, n);

    // u = normalize(cross(up, n))
    GLfloat u[3];
    cross(u, up, n);
    normalize(u, u);

    // v = cross(n, u)
    GLfloat v[3];
    cross(v, n, u);

    // Matrix aufbauen (Column-Major Order)
    out[0] = u[0];
    out[1] = u[1];
    out[2] = u[2];
    out[3] = 0;

    out[4] = v[0];
    out[5] = v[1];
    out[6] = v[2];
    out[7] = 0;

    out[8] = n[0];
    out[9] = n[1];
    out[10] = n[2];
    out[11] = 0;

    // Translation: Welt relativ zum Eye verschieben
    out[12] = -(u[0] * eye[0] + u[1] * eye[1] + u[2] * eye[2]);
    out[13] = -(v[0] * eye[0] + v[1] * eye[1] + v[2] * eye[2]);
    out[14] = -(n[0] * eye[0] + n[1] * eye[1] + n[2] * eye[2]);
    out[15] = 1;
}

void perspective(GLfloat *out, float fovy, float aspect, float near, float far)
{
    float f = 1.0f / tanf(fovy / 2.0f);
    for (int i = 0; i < 16; i++)
        out[i] = 0;
    out[0] = f / aspect;
    out[5] = f;
    out[10] = (far + near) / (near - far);
    out[11] = -1.0f;                              // Korrektur: Hier -1.0f setzen
    out[14] = (2.0f * far * near) / (near - far); // Korrektur: Hier den Wert setzen
}