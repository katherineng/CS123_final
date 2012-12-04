#ifndef ASTEROID_H
#define ASTEROID_H

#include <QGLWidget>
#include "CS123Algebra.h"
class Asteroid
{
public:
    Asteroid();
    virtual ~Asteroid();
    GLUquadric *m_quadric;
    void draw(float fps, float elapsed);
    Vector4 m_translation;
};

#endif // ASTEROID_H
