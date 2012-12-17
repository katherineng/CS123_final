#ifndef ASTEROID_H
#define ASTEROID_H

#include <QGLWidget>
#include "math/CS123Algebra.h"
#define RAD_CONVERT (M_PI / 180.0)

class Asteroid
{
public:
    Asteroid();
    virtual ~Asteroid();

    GLUquadric *m_quadric;
    Vector4 m_translation;
    Vector4 m_position;

    double m_radius;
    GLint *m_displacement;

    void draw(float elapsed);
    Vector4 getPosition();
    void setRadius(float r);
    double getRadius();

};

#endif // ASTEROID_H
