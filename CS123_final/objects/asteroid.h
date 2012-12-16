#ifndef ASTEROID_H
#define ASTEROID_H

#include <QGLWidget>
#include "CS123Algebra.h"
#define RAD_CONVERT (M_PI / 180.0)

class Asteroid
{
public:
    Asteroid();
    virtual ~Asteroid();

    GLUquadric *m_quadric;
    Vector4 m_translation;
    Vector4 m_position;
    Matrix4x4 m_scale;

    double m_radius;
    double m_seed;
    GLint *m_displacement;

    void draw(float fps, float elapsed);
    Vector4 getPosition();
    void setRadius(float r);
    double getRadius();
    GLuint m_texture;

};

#endif // ASTEROID_H