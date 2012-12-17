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


    /*! given the time elapsed, renders asteroid to the scene */
    void draw(float elapsed);

    /*! getter for m_position */
    Vector4 getPosition();

    /*! setter for m_radius */
    void setRadius(float r);

    /*! getter for m_radius */
    double getRadius();

};

#endif // ASTEROID_H
