#include "asteroid.h"
Asteroid::Asteroid()
{
    // Get a new quadrics obect
    m_quadric = gluNewQuadric();

    m_translation.x = rand()/RAND_MAX;
    m_translation.y = rand()/RAND_MAX;
    m_translation.z = rand()/RAND_MAX;
    m_translation.w = 0.0;

}

Asteroid::~Asteroid(){
    gluDeleteQuadric(m_quadric);
}

void Asteroid::draw(float fps, float elapsed){
    glPushMatrix();
    //glRotatef(90*time, 0, 0, 1);
    glTranslatef(m_translation.x+elapsed/fps, m_translation.y+elapsed/fps, m_translation.z+elapsed/fps);
    gluSphere(m_quadric, 1.0f, 10,10);
    glPopMatrix();
}
