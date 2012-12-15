#include "particleemitter.h"
#include <iostream>
using namespace std;

ParticleEmitter::ParticleEmitter(GLuint textureId, Vector3 color, Vector3 velocity,
                                 Vector3 force, float scale, float fuzziness, float speed,
                                 unsigned maxParticles) :
                    m_maxParticles(maxParticles), m_textureID(textureId), m_speed(speed),
                    m_fuzziness(fuzziness), m_scale(scale), m_color(color), m_velocity(velocity),
                    m_force(force)
{

    m_quadric = gluNewQuadric();
   // addExplosion(Vector4(0,0,0,0));

}

ParticleEmitter::~ParticleEmitter()
{
    foreach(Particle *particleList, m_particles)
        delete[] particleList;

    /*if (m_particles)
    {
        delete[] m_particles;
        m_particles = 0;
    }*/

    delete m_quadric;
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
        particles[i].active = false;
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
    int size;
    for (int j = 0; j < m_particles.size(); j++){
        m_time[j]++;
        particles = m_particles[j];
        size = m_numParticles[j];
        for(int i = 0; i < size; ++i)
        {
            if (particles[i].active){
                particles[i].pos.x += fmod(particles[i].dir.x * m_speed,.2);
                particles[i].pos.y += fmod(particles[i].dir.y * m_speed,.2);
                particles[i].pos.z += fmod(particles[i].dir.z * m_speed,.2);
                particles[i].dir += particles[i].force;
                particles[i].life  =particles[i].life - particles[i].decay;
                if (particles[i].life < 0){
                    particles[i].active = false;
                }
            } else {
                particles[i].active = true;
                resetParticle(i, particles);
            }
        }
    }
}

/**
  * You need to fill this in.
  *
  * Draws each particle as a small, texture-mapped square of side-length m_scale.
  * Each square should be in the X/Y plane at Z = the particle's position's Z-coordinate.
  */
void ParticleEmitter::drawParticles()
{
    //Put your code here


    Vector4 translate;
    Particle *particles;
    int size = explosionLocations.size();
    int partNum;
    int time;
    for (int j = 0; j < size; j++){
        time = m_time[j];
        if (time > 3000){
            deleteParticles(j);
        } else {
            translate = explosionLocations[j];
            particles = m_particles[j];
            glEnable(GL_BLEND);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, m_textureID);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glDepthMask(false);
            partNum =  m_numParticles[j];
            for (int i = 0; i < partNum; i++){

                if(particles[i].active){
                    glPushMatrix();
                    glPushAttrib(GL_CURRENT_BIT);
                    glTranslatef(translate.x, translate.y, translate.z);
                    glColor4f(particles[i].color.x,particles[i].color.y, particles[i].color.z, particles[i].life);
                    glBegin(GL_POINTS);


                    glPointSize(1000);
                    glVertex3f(particles[i].pos.x, particles[i].pos.y, particles[i].pos.z);

                    glEnd();
                    glPopAttrib();
                    glPopMatrix();
                }

            }
            m_numParticles[j] -= (int) 400;
            glDisable(GL_BLEND);
            glDepthMask(true);
            glAccum(GL_MULT,.95);
            glAccum(GL_ACCUM, .05);
            glAccum(GL_RETURN, 1);

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
