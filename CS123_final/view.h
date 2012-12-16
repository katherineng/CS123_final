#ifndef VIEW_H
#define VIEW_H

#include <qgl.h>
#include <QTime>
#include <QTimer>
#include <QGLWidget>
#include <QHash>
#include <QString>
#include <QTimer>
#include <QFile>

#include "camera.h"
#include "asteroid.h"
#include "selectionrecorder.h"
#include "particle/particleemitter.h"

class QGLShaderProgram;
class QGLFramebufferObject;

class View : public QGLWidget
{
    Q_OBJECT

public:
    View(QWidget *parent);
    ~View();

private:
    //GUI stuff
    Vector2 m_prevMousePos;
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    QFont m_font; // font for rendering text

    // Initialization code
    void initializeResources();
    void loadCubeMap();
    void createShaderPrograms();
    void createFramebufferObjects(int width, int height);

    // Drawing code
    void applyOrthogonalCamera(float width, float height);
    void applyPerspectiveCamera(float width, float height);
    void renderTexturedQuad(int width, int height);
    void renderScene();
    void renderSkybox();
    void paintText();

    //Time stuff
    QTime m_time;
    QTimer m_timer;
    QTime m_clock;
    int m_prevTime;
    float m_prevFps, m_fps, m_increment;
    OrbitCamera m_camera;

    // Skybox
    GLuint loadCubeMap(QList<QFile *> files);
    GLuint loadSkybox();
    QHash<QString, QGLFramebufferObject *> m_framebufferObjects; // hash map of all framebuffer objects

    // Textures
    GLuint m_skybox; // skybox call list ID
    GLuint m_cubeMap; // cubeMap texture ID
    GLuint m_texture; //texture of asteroid
    GLuint loadTexture(const QString &path);

    //Shader
    QHash<QString, QGLShaderProgram *> m_shaderPrograms; // hash map of all shader programs
    QGLShaderProgram *newShaderProgram(const QGLContext *context, QString vertShader, QString fragShader);

    //Asteroids
    vector<Asteroid*> m_asteroids;
    void collisionDetection(bool explosions);
    void renderAsteroids();
    void deleteAsteroids();
    void initializeAsteroids(int init_asteroids);

    //Particle stuff
    ParticleEmitter *m_emitter;

    //user-toggable bools
    bool m_collisionEnabled;
    bool m_displacementEnabled;

private slots:
    void tick();
};

#endif // VIEW_H

