#include "asteroid.h"
#include <QFile>

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


    m_rows = 60;
    m_cols = 60;
    m_radius = .5;
    m_seed = rand();
    m_numVertices = (m_rows-1)*m_cols + 2;
    m_vertices = new Vector4[m_numVertices];
    m_displacement = new double[m_numVertices];
    m_displacedNormals = new Vector4[m_numVertices];
    calculateVertices();
    //setDisplacement();
   // setDisplacedNormals();
}

Asteroid::~Asteroid(){

    gluDeleteQuadric(m_quadric);
    delete[] m_vertices;
    delete[] m_displacedNormals;
    delete[] m_displacement;
}


void Asteroid::setRadius(float r) {
    m_radius = r;
}

double Asteroid::getRadius() {
    return m_radius;
}

Vector4 Asteroid::getPosition() {
        return m_position;
}

void Asteroid::setDisplacedNormals(){
    for (int i = 0; i < m_numVertices; i++){
        m_displacedNormals[i] = calculateDisplacedNormal(i);
    }
}

Vector4 Asteroid::calculateDisplacedNormal(int i){
    if (i == 0){ // at the tip
    } else if (i == m_numVertices) { // at the bottom

    } else if ((i-1) % m_rows == 0){ // at left border

    } else if ((i-2) % m_rows == 0){ // at right border
    } else { //normal case

    }
    return Vector4(0,0,0,0);
}

void Asteroid::setDisplacement(){
    for (int i = 1; i < m_numVertices-1; i++){
        m_displacement[i] = (double) rand() / (double) RAND_MAX;
    }
}

void Asteroid::calculateVertices(){
    double latIncr = 180.0/m_cols;    // how much latitude increases with each iteration
    double longIncr = 360.0/m_rows;   // how much longitutde increase with each iteration
    double lat = latIncr;              // current latitutde
    double longi = 0;
    int col, row;
    double x,y,z;
    m_vertices[0] = Vector4(0,.5,0,1);
    for(row = 0; row < m_rows - 1; row++, lat += latIncr){
        longi = 0;
        for (col = 0; col < m_cols; col++, longi += longIncr){
            x = m_radius * sin(RAD_CONVERT*lat) * cos(RAD_CONVERT*longi);
            y = m_radius * cos(RAD_CONVERT*lat);
            z = m_radius * sin(RAD_CONVERT*lat) * sin(RAD_CONVERT*longi);
            m_vertices[1+ row*m_rows + col] = Vector4(x,y,z,1.0);
        }
    }
    m_vertices[m_numVertices-1] = Vector4(0,-.5,0,1);
}

void Asteroid::drawBody(){
    //draw body
    int row, col;
    Vector4 currVec;
    for(row = 0; row < m_rows - 2; row++){
        for (col = 0; col < m_cols-1; col++){

            glBegin(GL_TRIANGLES);

            currVec = m_vertices[1+row*m_rows+col];
            glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
            glVertex3f(currVec.x, currVec.y, currVec.z);

            currVec = m_vertices[1+(row+1)*m_rows+col];
            glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
            glVertex3f(currVec.x, currVec.y, currVec.z);

            currVec = m_vertices[1+row*m_rows+col+1];
            glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
            glVertex3f(currVec.x, currVec.y, currVec.z);

            glEnd();

          glBegin(GL_TRIANGLES);

            currVec = m_vertices[1+row*m_rows+col+1];
            glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
            glVertex3f(currVec.x, currVec.y, currVec.z);

            currVec = m_vertices[1+(row+1)*m_rows+col];
            glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
            glVertex3f(currVec.x, currVec.y, currVec.z);

            currVec = m_vertices[1+(row+1)*m_rows+col+1];
            glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
            glVertex3f(currVec.x, currVec.y, currVec.z);

            glEnd();
        }

        //handle wrap around

        glBegin(GL_TRIANGLES);

        currVec = m_vertices[row*m_rows+m_cols];
        glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
        glVertex3f(currVec.x, currVec.y, currVec.z);

        currVec = m_vertices[(row+1)*m_rows+m_cols];
        glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
        glVertex3f(currVec.x, currVec.y, currVec.z);

        currVec = m_vertices[1+row*m_rows];
        glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
        glVertex3f(currVec.x, currVec.y, currVec.z);

        glEnd();

        glBegin(GL_TRIANGLES);

        currVec = m_vertices[(row+1)*m_rows+m_cols];
        glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
        glVertex3f(currVec.x, currVec.y, currVec.z);

        currVec = m_vertices[1+(row+1)*m_rows];
        glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
        glVertex3f(currVec.x, currVec.y, currVec.z);

        currVec = m_vertices[1+row*m_rows];
        glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
        glVertex3f(currVec.x, currVec.y, currVec.z);

        glEnd();
    }

}

void Asteroid::drawBottom(){

    int row, col;
    Vector4 currVec;
    //draw bottom
    for (col = 0; col < m_cols-1; col++){
        glBegin(GL_TRIANGLES);

        currVec = m_vertices[1+m_cols*(m_rows -2)+col];
        glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
        glVertex3f(currVec.x, currVec.y, currVec.z);

        currVec = m_vertices[m_numVertices-1];
        glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
        glVertex3f(currVec.x, currVec.y, currVec.z);

        currVec = m_vertices[1+m_cols*(m_rows -2)+col+1];
        glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
        glVertex3f(currVec.x, currVec.y, currVec.z);

        glEnd();
    }

    glBegin(GL_TRIANGLES);

    currVec = m_vertices[m_cols*(m_rows -2)+m_cols];
    glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
    glVertex3f(currVec.x, currVec.y, currVec.z);

    currVec = m_vertices[m_numVertices-1];
    glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
    glVertex3f(currVec.x, currVec.y, currVec.z);

    currVec = m_vertices[2+m_cols*(m_rows -2)];
    glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
    glVertex3f(currVec.x, currVec.y, currVec.z);
    glEnd();

}

void Asteroid::drawTop(){
    //draw top
    int row, col;
    Vector4 currVec;
    for (col = 0; col < m_cols-1; col++){
        glBegin(GL_TRIANGLES);

        currVec = m_vertices[1];
        glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
        glVertex3f(currVec.x, currVec.y, currVec.z);

        currVec = m_vertices[1+col];
        glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
        glVertex3f(currVec.x, currVec.y, currVec.z);

        currVec = m_vertices[2+col];
        glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
        glVertex3f(currVec.x, currVec.y, currVec.z);

        glEnd();
    }

    glBegin(GL_TRIANGLES);

    currVec = m_vertices[1];
    glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
    glVertex3f(currVec.x, currVec.y, currVec.z);

    currVec = m_vertices[m_cols];
    glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
    glVertex3f(currVec.x, currVec.y, currVec.z);

    currVec = m_vertices[2];
    glNormal3f(currVec.x * 2,currVec.y * 2, currVec.z * 2);
    glVertex3f(currVec.x, currVec.y, currVec.z);

    glEnd();
}

void Asteroid::draw(float fps, float elapsed){

    double matrix[16];
    m_scale.getTranspose().fillArray(matrix);

  //  glEnable(GL_LIGHTING);
//    glMultMatrixd(matrix);
   // m_position += m_translation;
   // glRotatef(90+elapsed, m_position.x,m_position.y,m_position.z);
  //  glTranslatef(m_position.x * (elapsed/1000000), m_position.y*(elapsed/1000000), m_position.z*(elapsed/1000000));

  //  drawTop();
   // drawBody();
    //drawBottom();

    gluSphere(m_quadric, .5f, 30,30);

  //  glDisable(GL_LIGHTING);
}
