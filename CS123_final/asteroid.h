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

    Vector4 *m_displacedNormals;
    Vector4 *m_vertices;
    double *m_displacement;

    int m_numVertices;
    int m_rows;
    int m_cols;
    double m_radius;

    void draw(float fps, float elapsed);
    void drawTop();
    void drawBody();
    void drawBottom();
    void setDisplacement();
    void setDisplacedNormals();
    Vector4 calculateDisplacedNormal(int i);
    void calculateVertices();

};

#endif // ASTEROID_H
