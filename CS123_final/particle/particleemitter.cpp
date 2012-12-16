#include "particleemitter.h"
#include <iostream>

#include <QApplication>
#include <QKeyEvent>
#include <iostream>
#include <QFileDialog>
#include <QGLFramebufferObject>
#include <QGLShaderProgram>
#include <QMouseEvent>
#include <QTime>
#include <QTimer>
#include <QWheelEvent>
#include <GL/glu.h>
#include <vector>
using namespace std;

ParticleEmitter::ParticleEmitter(GLuint textureId, Vector3 color, Vector3 velocity,
                                 Vector3 force, float scale, float fuzziness, float speed,
                                 unsigned maxParticles) :
                    m_maxParticles(maxParticles), m_textureID(textureId), m_speed(speed),
                    m_fuzziness(fuzziness), m_scale(scale), m_color(color), m_velocity(velocity),
                    m_force(force)
{
    QFile file2("textures/smoke.png");
    QImage image2, texture2;
    image2.load(file2.fileName());
    texture2 = QGLWidget::convertToGLFormat(image2);
    //Put your code here
    glGenTextures(1, &m_smokeTex);
    glBindTexture(GL_TEXTURE_2D, m_smokeTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture2.width(), texture2.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture2.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   // gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture.width(), texture.height(), GL_RGB, GL_UNSIGNED_BYTE, texture.bits());
    glBindTexture(GL_TEXTURE_2D, 0);
    maxTime = 2000;
}

ParticleEmitter::~ParticleEmitter()
{
    foreach(Particle *particleList, m_particles)
        delete[] particleList;

    glDeleteTextures(1, &m_textureID);
}

/**
  * You need to fill this in.
  *
  * Resets the particle at the given index to its initial state. Should reset the
  * position, direction, force, color, life, and decay of the particle.
  */
void ParticleEmitter::resetParticle(int i, Particle *particles)
{
    particles[i].pos.x = 0;
    particles[i].pos.y = 0;
    particles[i].pos.z = 0;
    //Continue filling out code here
    particles[i].life = 1.0;
    particles[i].decay = urand(.0025, .15);
    particles[i].color = m_color;
    particles[i].force.x = urand(-m_fuzziness*.01, m_fuzziness *.01 + m_force.x);
    particles[i].force.y = urand(-m_fuzziness*.01, m_fuzziness *.01 + m_force.y);
    particles[i].force.z = urand(-m_fuzziness*.01, m_fuzziness *.01 + m_force.z);
    particles[i].dir.x = urand(-m_fuzziness, m_fuzziness + m_velocity.x);
    particles[i].dir.y = urand(-m_fuzziness, m_fuzziness + m_velocity.y);
    particles[i].dir.z = urand(-m_fuzziness, m_fuzziness + m_velocity.z);
}

/**
  * Resets all particles in this emitter to their initial states
  */
void ParticleEmitter::resetParticles(Particle *particles)
{
    for (int i = 0; i < m_maxParticles; i++)
        resetParticle(i, particles);

    for (int i = 0; i < m_maxParticles; ++i)
        particles[i].active = true;
}

Vector3 ParticleEmitter::lerp(Vector3 begin, Vector3 end, float percent){

    float red = begin.x - (percent * (begin.x - end.x));
    float green = begin.y - (percent * (begin.y-end.y));
    float blue = begin.z -  (percent * (begin.z - end.z));

    return Vector3(red, green, blue);
}

/**
  * You need to fill this in.
  *
  * Performs one step of the particle simulation. Should perform all physics
  * calculations and maintain the life property of each particle.
  */
void ParticleEmitter::updateParticles()
{
    Particle *particles;
    int size, time;
    for (int j = 0; j < m_particles.size(); j++){
        m_time[j]++;
        time =  m_time[j];
        particles = m_particles[j];
        size = m_numParticles[j];
        for(int i = 0; i < size; ++i)
        {
            if (particles[i].active){
                particles[i].pos.x += fmod(particles[i].dir.x * m_speed, .2);
                particles[i].pos.y += fmod(particles[i].dir.y * m_speed, .2);
                particles[i].pos.z += fmod(particles[i].dir.z * m_speed, .2);
                particles[i].dir += particles[i].force;
                particles[i].color = lerp(Vector3(1,1,1), Vector3(1, 0.5, 0), time/100.);
                float distance = sqrt(pow(particles[i].pos.x, 2) + pow(particles[i].pos.y, 2) + pow(particles[i].pos.z, 2));
                particles[i].life  =particles[i].life - particles[i].decay;
               // if (distance > 2.0)
                 //   particles[i].active = false;
              /*  if (particles[i].life < 0){
                    particles[i].active = false;
                }*/
            } /*else {
                particles[i].active = true;
                resetParticle(i, particles);
            }*/
        }
    }
}

void ParticleEmitter::renderTexturedQuad(int width, int height) {
    // Clamp value to edge of texture when texture index is out of bounds
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Draw the  quad
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);

    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(0.0f, height);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(width, height);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(width, 0.0f);


    glEnd();
}

/**
  * You need to fill this in.
  *
  * Draws each particle as a small, texture-mapped square of side-length m_scale.
  * Each square should be in the X/Y plane at Z = the particle's position's Z-coordinate.
  */
void ParticleEmitter::drawParticles(double theta, double phi)
{
    Vector3 u;

    Vector4 translate;
    Particle *particles;
    int size = explosionLocations.size();
    int partNum;
    int time;
    for (int j = 0; j < size; j++){
        time = m_time[j];
        if (time > maxTime){
            deleteParticles(j);
        } else {
            translate = explosionLocations[j];
            particles = m_particles[j];
            glEnable(GL_BLEND);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, m_smokeTex);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(false);
            partNum =  m_numParticles[j];

            for (int i = 0; i < partNum; i++){

                if(particles[i].active && particles[i].life!=0){
                    glPushMatrix();

                /*    l = -(l - Vector3(particles[i].pos.x,  particles[i].pos.y, particles[i].pos.z).getNormalized());
                    r= r.cross(l);
                    u = l.cross(r);


                    float modelview[16];
                    modelview[0]=r.x;   modelview[1]=u.x;   modelview[2]=l.x;   modelview[3]=particles[i].pos.x;
                    modelview[4]=r.y;   modelview[5]=u.y;   modelview[6]=l.y;   modelview[7]=particles[i].pos.y;
                    modelview[8]=r.z;   modelview[9]=u.z;   modelview[10]=l.z;   modelview[11]=particles[i].pos.z;
                    modelview[12]=0;     modelview[13]=0;     modelview[14]=0;     modelview[15]=1;

                    float modelview[16];
                    int i,j;


                    // get the current modelview matrix
                    glGetFloatv(GL_MODELVIEW_MATRIX , modelview);

                    // undo all rotations
                    // beware all scaling is lost as well
                    for( i=0; i<3; i++ )
                            for( j=0; j<3; j++ ) {
                                    if ( i==j )
                                            modelview[i*4+j] = 1.0;
                                    else
                                            modelview[i*4+j] = 0.0;
                            }

                    // set the modelview with no rotations and scaling
                    glLoadMatrixf(modelview);*/

                    /*theta += delta.x * 0.01f;
                    phi += delta.y * 0.01;

                     -= floorf(theta / (2*M_PI)) * (2*M_PI);*/
                    //double phi = MAX(0.01 - M_PI / 2, MIN(M_PI / 2 - 0.01, phi));
                    glPushAttrib(GL_CURRENT_BIT);
                    glRotatef(fmod((270 - theta), 360), 0.,1.,0.);
                    glColor4f(particles[i].color.x,particles[i].color.y, particles[i].color.z, particles[i].life);
                    glTranslatef(translate.x + particles[i].pos.x, translate.y + particles[i].pos.y, translate.z + particles[i].pos.z);
                    renderTexturedQuad(1, 1);
                    glPopAttrib();
                    glPopMatrix();
                }

            }
            //m_numParticles[j] -= (int) 1000;
            glDisable(GL_BLEND);
            glDepthMask(true);
         //   glAccum(GL_MULT,.95);
        //    glAccum(GL_ACCUM, .05);
         //   glAccum(GL_RETURN, 1);

        }
    }

}

void ParticleEmitter::deleteParticles(int index){
    m_time.erase(m_time.begin() + index);
    m_particles.erase(m_particles.begin()+index);
    explosionLocations.erase(explosionLocations.begin() + index);
    m_numParticles.erase(m_numParticles.begin() + index);
}

void ParticleEmitter::addExplosion(Vector4 location){
    explosionLocations.push_back(location);
    Particle *particleList= new Particle[m_maxParticles];
    resetParticles(particleList);
    m_particles.push_back(particleList);
    m_time.push_back(0);
    m_numParticles.push_back(m_maxParticles);
}
