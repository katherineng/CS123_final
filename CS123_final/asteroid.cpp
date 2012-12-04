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

}

Asteroid::~Asteroid(){
    gluDeleteQuadric(m_quadric);
}

void Asteroid::draw(float fps, float elapsed){

    glPushMatrix(); glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // Set up global (ambient) lighting
    GLfloat global_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

    // Set up GL_LIGHT0 with a position and lighting properties
    GLfloat ambientLight[] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0, 1.0f };
    GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat position[] = { 2.0f, 2.0f, 2.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    m_position += m_translation;
    glPushAttrib( GL_CURRENT_BIT );
    glColor3f(0.5f, 0.0f, 1.0f);
    glRotatef(90+elapsed, m_position.x,m_position.y,m_position.z);
    glTranslatef(m_position.x * (elapsed/1000000), m_position.y*(elapsed/1000000), m_position.z*(elapsed/1000000));
    gluSphere(m_quadric, 1.0f, 10,10);
    glPopAttrib();
    glPopMatrix();
}
