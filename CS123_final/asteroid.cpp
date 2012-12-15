#include "asteroid.h"
#include <QFile>
#include <QImage>

Asteroid::Asteroid()
{
    // Get a new quadrics obect
    m_quadric = gluNewQuadric();

    m_translation.x = ((float) rand() / ((float) RAND_MAX/2.0) - 1.0);
    m_translation.y = ((float) rand() / ((float) RAND_MAX/2.0) - 1.0);
    m_translation.z = ((float) rand() / ((float) RAND_MAX/2.0) - 1.0);
    m_translation.w = 0.0;

    m_translation.getNormalized();
    m_translation = m_translation/100;


    float max = 5;
    float min = -max;
    m_position = Vector4(fmod((float) rand(), (max-min))+min, fmod((float) rand(), (max-min))+min, fmod((float) rand(), (max-min))+min, 0.0);

    double xScale = (float) rand() / ((float) RAND_MAX) + 1.0;
    double yScale = (float) rand() / ((float) RAND_MAX) + 1.0;
    double zScale = (float) rand() / ((float) RAND_MAX) + 1.0;

    m_scale = Matrix4x4(xScale, 0, 0, 0,
                         0, yScale, 0, 0,
                         0, 0, zScale, 0,
                         0, 0, 0, 1);

    m_radius = .5;
    m_seed = rand();

    m_displacement = new int[512];

    for (int i = 0; i < 512; i++) {
        m_displacement[i] = (GLint) rand() % 256;
    }

    gluQuadricTexture(m_quadric, GL_TRUE);
    gluQuadricNormals(m_quadric, GL_SMOOTH);
}

Asteroid::~Asteroid(){

    gluDeleteQuadric(m_quadric);
    delete[] m_displacement;

}


Vector4 Asteroid::getPosition(){
    return m_position;

}

void Asteroid::draw(float fps, float elapsed){

    double matrix[16];
    m_scale.getTranspose().fillArray(matrix);

    //glEnable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
   glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
    glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glPushMatrix();
   // glMultMatrixd(matrix);
    m_position += m_translation;

    glRotatef(90+elapsed, m_position.x,m_position.y,m_position.z);
    glTranslatef(m_position.x, m_position.y, m_position.z);
    gluSphere(m_quadric, m_radius, 10, 10);

    glPopMatrix();
    glDisable(GL_TEXTURE_GEN_S);
   glDisable(GL_TEXTURE_GEN_T);



}

void Asteroid::setRadius(float r) {
    m_radius = r;
}

double Asteroid::getRadius() {
    return m_radius;
}


