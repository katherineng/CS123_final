#include "asteroid.h"
Asteroid::Asteroid()
{
    // Get a new quadrics obect
    m_quadric = gluNewQuadric();

    m_translation.x = (float) rand() / ((float) RAND_MAX/2.0) - 1.0;
    m_translation.y = (float) rand() / ((float) RAND_MAX/2.0) - 1.0;
    m_translation.z = (float) rand() / ((float) RAND_MAX/2.0) - 1.0;
    m_translation.w = 0.0;

    m_position = m_translation;

    double xScale = (float) rand() / ((float) RAND_MAX) + 1.0;
    double yScale = (float) rand() / ((float) RAND_MAX) + 1.0;
    double zScale = (float) rand() / ((float) RAND_MAX) + 1.0;

    m_scale = Matrix4x4(xScale, 0, 0, 0,
                         0, yScale, 0, 0,
                         0, 0, zScale, 0,
                         0, 0, 0, 1);
}

Asteroid::~Asteroid(){
    gluDeleteQuadric(m_quadric);
}

void Asteroid::draw(float fps, float elapsed){

    double matrix[16];
    m_scale.getTranspose().fillArray(matrix);

    glEnable(GL_LIGHTING);
    glPushMatrix();
    glMultMatrixd(matrix);
    m_position += m_translation;
    glPushAttrib( GL_CURRENT_BIT );
    glColor3f(0.5f, 0.0f, 1.0f);
    glRotatef(90+elapsed, m_position.x,m_position.y,m_position.z);
    glTranslatef(m_position.x * (elapsed/1000000), m_position.y*(elapsed/1000000), m_position.z*(elapsed/1000000));
    gluSphere(m_quadric, .5f, 10,10);
    glPopAttrib();
    glPopMatrix();
    glDisable(GL_LIGHTING);
}
