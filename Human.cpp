#include <Windows.h>
#include <stdlib.h> 
#include <math.h> 
#include <stdio.h>
#include <GL/GLU.h>
#include <glut.h>
#include <glaux.h>
#include <direct.h>

double user_theta = 0;
double user_height = 0;
const float M_PI = 3.14159265358979323846;

void drawSphere(double r, int lats, int longs, float red, float green, float blue) {
    int i, j;
    glColor3f(red, green, blue); // Set the color for the sphere
    for (i = 0; i <= lats; i++) {
        double lat0 = M_PI * (-0.5 + (double)(i - 1) / lats);
        double z0 = sin(lat0);
        double zr0 = cos(lat0);

        double lat1 = M_PI * (-0.5 + (double)i / lats);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (j = 0; j <= longs; j++) {
            double lng = 2 * M_PI * (double)(j - 1) / longs;
            double x = cos(lng);
            double y = sin(lng);

            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(r * x * zr0, r * y * zr0, r * z0);
            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(r * x * zr1, r * y * zr1, r * z1);
        }
        glEnd();
    }
}

void drawSmallSphereAttached(double r_big, double r_small_ratio, double theta) {
    double r_small = r_big * r_small_ratio; // Calculate small sphere radius based on the big sphere's radius
    // Calculate position so the small sphere's center is at the surface of the big sphere
    double x = r_big * cos(theta);
    double y = r_big * sin(theta);
    // Since we want the small sphere to be half embedded, we don't need to adjust z by the small sphere's radius
    double z = 0; // Keep z as 0 assuming we're placing it at the equator for simplicity

    glPushMatrix();
    glTranslatef(x, y, z); // Move to the new position
    drawSphere(r_small, 10, 10, 0.0, 0.0, 0.0); // Draw the small sphere with color black
    glPopMatrix();
}


void computeLocation() {
    double x = 2 * cos(user_theta);
    double y = 2 * sin(user_theta);
    double z = user_height;
    double d = sqrt(x * x + y * y + z * z);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-d * 0.5, d * 0.5, -d * 0.5, d * 0.5, d - 1.1, d + 1.1);
    gluLookAt(x, y, z, 0, 0, 0, 0, 0, 1);
}

void init() {
    GLfloat sun_direction[] = { 0.0, 2.0, -1.0, 1.0 };
    GLfloat sun_intensity[] = { 0.7, 0.7, 0.7, 1.0 };
    GLfloat ambient_intensity[] = { 0.7, 0.7, 0.7, 1.0 };

    glClearColor(1.0, 1.0, 1.0, 0.0);
    computeLocation();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_intensity);

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, sun_direction);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_intensity);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);

    drawSphere(1.0, 10, 10, 0.9725, 0.8431, 0.5216); // Draw the large sphere with its color
    drawSmallSphereAttached(1.0, 0.1, user_theta); // Draw the small sphere attached with its radius and color

    glutSwapBuffers();
}

void idle() {
    static int lastTime = 0;
    int time = glutGet(GLUT_ELAPSED_TIME);

    if (lastTime == 0 || time >= lastTime + 40) {
        lastTime = time;
        glutPostRedisplay();
    }
}

void visible(int vis) {
    glutIdleFunc(vis == GLUT_VISIBLE ? idle : NULL);
}

void special(int k, int x, int y) {
    switch (k) {
    case GLUT_KEY_UP:    user_height += 0.1; break;
    case GLUT_KEY_DOWN:  user_height -= 0.1; break;
    case GLUT_KEY_LEFT:  user_theta += 0.1; break;
    case GLUT_KEY_RIGHT: user_theta -= 0.1; break;
    }
    computeLocation();
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(50, 100);
    glutInitWindowSize(300, 300);
    glutCreateWindow("Sphere");

    init();
    glutDisplayFunc(draw);
    glutVisibilityFunc(visible);
    glutSpecialFunc(special);
    glutMainLoop();
    return 0;
}
