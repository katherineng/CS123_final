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

class QGLShaderProgram;
class QGLFramebufferObject;


class View : public QGLWidget
{
    Q_OBJECT

public:
    View(QWidget *parent);
    ~View();
    GLuint loadCubeMap(QList<QFile *> files);
    GLuint loadSkybox();

    //from lab09

    void wheelEvent(QWheelEvent *event);

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
    void renderAsteroids();
    void paintText();

protected:
    vector<Asteroid*> m_asteroids;

private:
    QTime m_time;
    QTimer m_timer;
    QTime m_clock;


    Asteroid *asteroid;

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    int m_prevTime;
    float m_prevFps, m_fps, m_increment;
    Vector2 m_prevMousePos;
    OrbitCamera m_camera;

    // Resources
    QHash<QString, QGLShaderProgram *> m_shaderPrograms; // hash map of all shader programs
    QHash<QString, QGLFramebufferObject *> m_framebufferObjects; // hash map of all framebuffer objects
    GLuint m_skybox; // skybox call list ID
    GLuint m_cubeMap; // cubeMap texture ID
    GLuint m_texture;
    QFont m_font; // font for rendering text
    QGLShaderProgram *newShaderProgram(const QGLContext *context, QString vertShader, QString fragShader);
private slots:
    void tick();
};

#endif // VIEW_H

