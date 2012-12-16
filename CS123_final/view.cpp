#include "view.h"
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
using std::cout;
using std::endl;



const int p[] = { 151,160,137,91,90,15,
   131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
   190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
   88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
   77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
   102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
   135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
   5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
   223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
   129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
   251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
   49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
   138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
   };


extern "C"
{
    extern void APIENTRY glActiveTexture(GLenum);
}

static const int MAX_FPS = 120;
View::View(QWidget *parent) : QGLWidget(parent)
{
    // View needs all mouse move events, not just mouse drag events
    setMouseTracking(true);

    // Hide the cursor since this is a fullscreen app
  //  setCursor(Qt::BlankCursor);

    // View needs keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    // The game loop is implemented using a timer
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));

    //bloom
    m_camera.center = Vector3(0., 0., 0.);
    m_camera.up = Vector3(0.f, 1.f, 0.f);
    m_camera.zoom = 3.5f;
    m_camera.theta = M_PI * 1.5f, m_camera.phi = 0.2f;
    m_camera.fovy = 60.f;

    initializeAsteroids(40);
    m_texture = 0;

   // asteroid = new Asteroid();
}

View::~View()
{
    foreach (QGLShaderProgram *sp, m_shaderPrograms)
        delete sp;
    foreach (QGLFramebufferObject *fbo, m_framebufferObjects)
        delete fbo;
    glDeleteLists(m_skybox, 1);
    const_cast<QGLContext *>(context())->deleteTexture(m_cubeMap);
    deleteAsteroids();
    delete m_emitter;
}

GLuint View::loadTexture(const QString &path){
        QFile file(path);

        QImage image, texture;
        if(!file.exists()) return -1;
        image.load(file.fileName());
        texture = QGLWidget::convertToGLFormat(image);
        //Put your code here
        GLuint id = 0;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width(), texture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        return id;
}

void View::deleteAsteroids(){

    m_asteroids.clear();
    /*foreach (Asteroid *a, m_asteroids) {
        delete a;
    }*/
}

void View::initializeAsteroids(int init_asteroids){
    int i;
    for (i = 0; i < init_asteroids; i++) {
        Asteroid *a = new Asteroid();
        m_asteroids.push_back(a);
    }

    int j, num_asteroids = m_asteroids.size();
    for (i = 0; i < num_asteroids; i++) {
        Vector4 pos1 = m_asteroids[i]->getPosition();
        float rad = m_asteroids[i]->getRadius();
        for (j = i + 1; j < num_asteroids; j++){
            Vector4 pos2 = m_asteroids[j]->getPosition();
            float collision_rad = rad + m_asteroids[j]->getRadius();
            float distance = sqrt(pow(pos2.x - pos1.x, 2) + pow(pos2.y - pos1.y, 2) + pow(pos2.z - pos1.z, 2));
            if (distance <= collision_rad) {
                delete m_asteroids[i];
                delete m_asteroids[j];
                m_asteroids.erase(m_asteroids.begin() + j);
                m_asteroids.erase(m_asteroids.begin() + i);
                num_asteroids -= 2;
                i--;
                break;
                //j = i+1;
            }
        }
    }
}

void View::initializeGL()
{
    // All OpenGL initialization *MUST* be done during or after this
    // method. Before this method is called, there is no active OpenGL
    // context and all OpenGL calls have no effect.

    // Start a timer that will try to get 60 frames per second (the actual
    // frame rate depends on the operating system and other running programs)
   // time.start();
   // timer.start(1000 / 60);

    // Center the mouse, which is explained more in mouseMoveEvent() below.
    // This needs to be done here because the mouse may be initially outside
    // the fullscreen window and will not automatically receive mouse move
    // events. This occurs if there are two monitors and the mouse is on the
    // secondary monitor.
    QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));
m_increment = 0;
    // bloom lab

    // Set up OpenGL
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glDisable(GL_DITHER);

    glDisable(GL_LIGHTING);
    //glEnable(GL_LIGHTING);
    //glShadeModel(GL_FLAT);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Load resources, including creating shader programs and framebuffer objects
    initializeResources();

    // Start the drawing timer
    m_timer.start(1000.0f / MAX_FPS);

}


/**
  Initialize all resources.
  This includes models, textures, call lists, shader programs, and framebuffer objects.
 **/
void View::initializeResources()
{
    cout << "Using OpenGL Version " << glGetString(GL_VERSION) << endl << endl;
    // Ideally we would now check to make sure all the OGL functions we use are supported
    // by the video card.  But that's a pain to do so we're not going to.
    cout << "--- Loading Resources ---" << endl;

//     m_dragon = loadObjModel("/course/cs123/bin/models/xyzrgb_dragon.obj");
//     cout << "Loaded dragon..." << endl;

    m_skybox = loadSkybox();
    cout << "Loaded skybox..." << endl;

    QFile file("textures/asteroid1.jpg");
    QImage image, texture;
    image.load(file.fileName());
    texture = QGLWidget::convertToGLFormat(image);
    //Put your code here
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width(), texture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   // gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture.width(), texture.height(), GL_RGB, GL_UNSIGNED_BYTE, texture.bits());
    glBindTexture(GL_TEXTURE_2D, 0);




    m_emitter = new ParticleEmitter(loadTexture("/textures/particle2.bmp"),
                                    Vector3(1.0f, 0.5f, 0.2f), //color
                                    Vector3(0.0f, 0.0001f, 0.0f), //velocity
                                    Vector3(0.0f, 0.0001f, 0.0f), //force
                                    100.f, //scaole
                                    10.0f, //fuzziness
                                    8.f / 10000.0f, //speed
                                    100); //max particles

    loadCubeMap();
    cout << "Loaded cube map..." << endl;

    createShaderPrograms();
    cout << "Loaded shader programs..." << endl;

    createFramebufferObjects(width(), height());
    cout << "Loaded framebuffer objects..." << endl;

    cout << " --- Finish Loading Resources ---" << endl;

}

/**
  Loads the cube map into video memory.

  @param files: a list of files containing the cube map images (should be length
  six) in order
  @return the assigned OpenGL id to the cube map
**/
GLuint View::loadCubeMap(QList<QFile *> files)
{
    Q_ASSERT(files.length() == 6);

    // Generate an ID
    GLuint id;
    glGenTextures(1, &id);

    // Bind the texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    // Load and build mipmaps for each image
    for (int i = 0; i < 6; ++i)
    {
        QImage image, texture;
        image.load(files[i]->fileName());
        image = image.mirrored(false, true);
        texture = QGLWidget::convertToGLFormat(image);
        texture = texture.scaledToWidth(1024, Qt::SmoothTransformation);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 3, 3, texture.width(), texture.height(), 0, GL_RGBA,GL_UNSIGNED_BYTE, texture.bits());
        gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 3, texture.width(), texture.height(), GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());
    }

    // Set filter when pixel occupies more than one texture element
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    // Set filter when pixel smaller than one texture element
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return id;
}


/**
    Creates a call list for a skybox
  **/
GLuint View::loadSkybox()
{
    GLuint id = glGenLists(1);
    glNewList(id, GL_COMPILE);

    // Be glad we wrote this for you...ugh.
    glBegin(GL_QUADS);
    float extent = 10000.f;
    glTexCoord3f( 1.0f, -1.0f, -1.0f); glVertex3f( extent, -extent, -extent);
    glTexCoord3f(-1.0f, -1.0f, -1.0f); glVertex3f(-extent, -extent, -extent);
    glTexCoord3f(-1.0f,  1.0f, -1.0f); glVertex3f(-extent,  extent, -extent);
    glTexCoord3f( 1.0f,  1.0f, -1.0f); glVertex3f( extent,  extent, -extent);
    glTexCoord3f( 1.0f, -1.0f,  1.0f); glVertex3f( extent, -extent,  extent);
    glTexCoord3f( 1.0f, -1.0f, -1.0f); glVertex3f( extent, -extent, -extent);
    glTexCoord3f( 1.0f,  1.0f, -1.0f); glVertex3f( extent,  extent, -extent);
    glTexCoord3f( 1.0f,  1.0f,  1.0f); glVertex3f( extent,  extent,  extent);
    glTexCoord3f(-1.0f, -1.0f,  1.0f); glVertex3f(-extent, -extent,  extent);
    glTexCoord3f( 1.0f, -1.0f,  1.0f); glVertex3f( extent, -extent,  extent);
    glTexCoord3f( 1.0f,  1.0f,  1.0f); glVertex3f( extent,  extent,  extent);
    glTexCoord3f(-1.0f,  1.0f,  1.0f); glVertex3f(-extent,  extent,  extent);
    glTexCoord3f(-1.0f, -1.0f, -1.0f); glVertex3f(-extent, -extent, -extent);
    glTexCoord3f(-1.0f, -1.0f,  1.0f); glVertex3f(-extent, -extent,  extent);
    glTexCoord3f(-1.0f,  1.0f,  1.0f); glVertex3f(-extent,  extent,  extent);
    glTexCoord3f(-1.0f,  1.0f, -1.0f); glVertex3f(-extent,  extent, -extent);
    glTexCoord3f(-1.0f,  1.0f, -1.0f); glVertex3f(-extent,  extent, -extent);
    glTexCoord3f(-1.0f,  1.0f,  1.0f); glVertex3f(-extent,  extent,  extent);
    glTexCoord3f( 1.0f,  1.0f,  1.0f); glVertex3f( extent,  extent,  extent);
    glTexCoord3f( 1.0f,  1.0f, -1.0f); glVertex3f( extent,  extent, -extent);
    glTexCoord3f(-1.0f, -1.0f, -1.0f); glVertex3f(-extent, -extent, -extent);
    glTexCoord3f(-1.0f, -1.0f,  1.0f); glVertex3f(-extent, -extent,  extent);
    glTexCoord3f( 1.0f, -1.0f,  1.0f); glVertex3f( extent, -extent,  extent);
    glTexCoord3f( 1.0f, -1.0f, -1.0f); glVertex3f( extent, -extent, -extent);
    glEnd();
    glEndList();

    return id;
}
int i = 0;
void View::paintGL()
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update the fps
    int time = m_clock.elapsed();
    m_fps = 1000.f / (time - m_prevTime);
    m_prevTime = time;
    int width = this->width();
    int height = this->height();

    // Render the scene to a framebuffer
    m_framebufferObjects["fbo_0"]->bind();
    applyPerspectiveCamera(width, height);
    renderScene();
    m_framebufferObjects["fbo_0"]->release();

    // Copy the rendered scene into framebuffer 1
    m_framebufferObjects["fbo_0"]->blitFramebuffer(m_framebufferObjects["fbo_1"],
                                                   QRect(0, 0, width, height), m_framebufferObjects["fbo_0"],
                                                   QRect(0, 0, width, height), GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // TODO: Step 0 - draw the scene to the screen as a textured quad

    applyOrthogonalCamera(width, height);
    glBindTexture(GL_TEXTURE_2D, m_framebufferObjects["fbo_1"]->texture());
    renderTexturedQuad(width, height);
    glBindTexture(GL_TEXTURE_2D, 0);

    paintText();
    // TODO: Implement the demo rendering here
}

void View::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void View::mousePressEvent(QMouseEvent *event)
{

        m_prevMousePos.x = event->x();
        m_prevMousePos.y = event->y();

}



void View::mouseMoveEvent(QMouseEvent *event)
{
    // This starter code implements mouse capture, which gives the change in
    // mouse position since the last mouse movement. The mouse needs to be
    // recentered after every movement because it might otherwise run into
    // the edge of the screen, which would stop the user from moving further
    // in that direction. Note that it is important to check that deltaX and
    // deltaY are not zero before recentering the mouse, otherwise there will
    // be an infinite loop of mouse move events.
  //  int deltaX = event->x() - width() / 2;
   // int deltaY = event->y() - height() / 2;
   // if (!deltaX && !deltaY) return;
   // QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));

        Vector2 pos(event->x(), event->y());
        if (event->buttons() & Qt::LeftButton || event->buttons() & Qt::RightButton)
        {
            m_camera.mouseMove(pos - m_prevMousePos);
        }
        m_prevMousePos = pos;

    // TODO: Handle mouse movements here
}

void View::mouseReleaseEvent(QMouseEvent *event)
{


}

void View::keyPressEvent(QKeyEvent *event)
{
   // if (event->key() == Qt::Key_Escape) QApplication::quit();
    switch(event->key())
    {
    case Qt::Key_S:
        {
        QImage qi = grabFrameBuffer(false);
        QString filter;
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("PNG Image (*.png)"), &filter);
        qi.save(QFileInfo(fileName).absoluteDir().absolutePath() + "/" + QFileInfo(fileName).baseName() + ".png", "PNG", 100);
        break;
    }
    case Qt::Key_A:
        m_asteroids.push_back(new Asteroid());
        break;
    case Qt::Key_R:
        deleteAsteroids();
        initializeAsteroids(40);
        break;

    }
    // TODO: Handle keyboard presses here
}

void View::keyReleaseEvent(QKeyEvent *event)
{
}

void View::tick()
{
    // Get the number of seconds since the last tick (variable update rate)
    float seconds = m_time.restart() * 0.001f;

    // TODO: Implement the demo update here

    // Flag this view for repainting (Qt will call paintGL() soon after)
    update();
}



/**
  Load a cube map for the skybox
 **/
void View::loadCubeMap()
{
    QList<QFile *> fileList;
    fileList.append(new QFile("textures/posx.jpg"));
    fileList.append(new QFile("textures/negx.jpg"));
    fileList.append(new QFile("textures/posz.jpg"));
    fileList.append(new QFile("textures/posz.jpg"));
    fileList.append(new QFile("textures/negz.jpg"));
    fileList.append(new QFile("textures/posz.jpg"));
    m_cubeMap = loadCubeMap(fileList);
}

/**
    Creates a shader program from both vert and frag shaders
  **/
QGLShaderProgram *View::newShaderProgram(const QGLContext *context, QString vertShader, QString fragShader)
{
    QGLShaderProgram *program = new QGLShaderProgram(context);
    program->addShaderFromSourceFile(QGLShader::Vertex, vertShader);
    program->addShaderFromSourceFile(QGLShader::Fragment, fragShader);
    !program->link();
    return program;
}

/**
  Create shader programs.
 **/
void View::createShaderPrograms()
{
    const QGLContext *ctx = context();
    m_shaderPrograms["displacement"] = newShaderProgram(ctx, "shaders/displacement.vert", "shaders/displacement.frag");
    m_shaderPrograms["particles"] = newShaderProgram(ctx, "shaders/particles.vert", "shaders/particles.frag");
}


/**
  Allocate framebuffer objects.

  @param width: the viewport width
  @param height: the viewport height
 **/
void View::createFramebufferObjects(int width, int height)
{
    // Allocate the main framebuffer object for Scing the scene to
    // This needs a depth attachment
    m_framebufferObjects["fbo_0"] = new QGLFramebufferObject(width, height, QGLFramebufferObject::Depth,
                                                             GL_TEXTURE_2D, GL_RGB16F_ARB);
    m_framebufferObjects["fbo_0"]->format().setSamples(16);
    // Allocate the secondary framebuffer obejcts for rendering textures to (post process effects)
    // These do not require depth attachments
    m_framebufferObjects["fbo_1"] = new QGLFramebufferObject(width, height, QGLFramebufferObject::NoAttachment,
                                                             GL_TEXTURE_2D, GL_RGB16F_ARB);

    m_framebufferObjects["fbo_2"] = new QGLFramebufferObject(width, height, QGLFramebufferObject::NoAttachment,
                                                             GL_TEXTURE_2D, GL_RGB16F_ARB);
}


/**
  Called to switch to an orthogonal OpenGL camera.
  Useful for rending a textured quad across the whole screen.

  @param width: the viewport width
  @param height: the viewport height
**/
void View::applyOrthogonalCamera(float width, float height)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.f, width, 0.f, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


/**
  Called to switch to a perspective OpenGL camera.

  @param width: the viewport width
  @param height: the viewport height
**/
void View::applyPerspectiveCamera(float width, float height)
{
    float ratio = ((float) width) / height;
    Vector3 dir;
    Vector3 x;
    dir = -x.fromAngles(m_camera.theta, m_camera.phi);
    Vector3 eye(m_camera.center - dir * m_camera.zoom);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(m_camera.fovy, ratio, 0.1f, 21000.f);
    gluLookAt(eye.x, eye.y, eye.z,
              eye.x + dir.x, eye.y + dir.y, eye.z + dir.z,
              m_camera.up.x, m_camera.up.y, m_camera.up.z);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void View::renderTexturedQuad(int width, int height) {
    // Clamp value to edge of texture when texture index is out of bounds
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Draw the  quad
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(width, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(width, height);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(0.0f, height);
    glEnd();
}
/**
  Renders the scene.  May be called multiple times by paintGL() if necessary.
**/
void View::renderScene()
{



    int i, j, num_asteroids = m_asteroids.size();
    for (i = 0; i < num_asteroids; i++) {
        Vector4 pos1 = m_asteroids[i]->getPosition();
        float rad = m_asteroids[i]->getRadius();
        for (j = i + 1; j < num_asteroids; j++){
            Vector4 pos2 = m_asteroids[j]->getPosition();
            float collision_rad = rad + m_asteroids[j]->getRadius();
            float distance = sqrt(pow(pos2.x - pos1.x, 2) + pow(pos2.y - pos1.y, 2) + pow(pos2.z - pos1.z, 2));
            if (distance <= collision_rad) {
                m_emitter->addExplosion(m_asteroids[i]->getPosition());
                delete m_asteroids[i];
                delete m_asteroids[j];
                m_asteroids.erase(m_asteroids.begin() + j);
                m_asteroids.erase(m_asteroids.begin() + i);
                num_asteroids -= 2;
                i--;
                break;
                //j = i+1;
            }
        }
    }


    m_increment++;
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMap);

    // Enable cube maps and draw the skybox
    glEnable(GL_TEXTURE_CUBE_MAP);
    glCallList(m_skybox);

    glBindTexture(GL_TEXTURE_CUBE_MAP,0);
    glDisable(GL_TEXTURE_CUBE_MAP);

    // Enable culling (back) faces for rendering the dragon
    glEnable(GL_CULL_FACE);

    // Render the dragon with the refraction shader bound
    m_emitter->updateParticles();       //Move the particles
    m_emitter->drawParticles();
    glEnable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    m_shaderPrograms["displacement"]->bind();
  //  m_shaderPrograms["displacement"]->setUniformValue("seed", (GLfloat)asteroid->m_seed);
    //m_shaderPrograms["displacement"]->setUniformValueArray("p", p, 256);
    m_shaderPrograms["displacement"]->setUniformValue("textureMap",GL_TEXTURE0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Enable alpha blending and render the texture to the screen
    renderAsteroids();
    m_shaderPrograms["displacement"]->release();
    glBindTexture(GL_TEXTURE_2D, 0);



    // Disable culling, depth testing and cube maps
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

}

//void View::renderExplosion(){

//    Vector4 translate;
//    Particle *particles;
//    int size = m_emitter->explosionLocations.size();
//    int partNum;
//    int time;
//    Vector3 pos;
//    for (int j = 0; j < size; j++){
//        time =m_emitter->m_time[j];
//        if (time > 3000){
//            m_emitter->deleteParticles(j);
//        } else {
//            translate = m_emitter->explosionLocations[j];
//            particles = m_emitter->m_particles[j];

//            glEnable(GL_BLEND);
//            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//            glEnable(GL_TEXTURE_2D);
//            glBindTexture(GL_TEXTURE_2D, m_smokeTex);
//           // glBindTexture(GL_TEXTURE_2D, m_emitter->m_textureID);
//            glDepthMask(false);

//            partNum =  m_emitter->m_numParticles[j];

//            for (int i = 0; i < partNum; i++){

//                if(particles[i].active){
//                    glPushMatrix();
//                   // m_shaderPrograms["particles"]->setUniformValue("translate", QVector3D(translate.x, translate.y, translate.z));
//                  //  m_shaderPrograms["particles"]->setUniformValue("color", QVector4D(particles[i].color.x,particles[i].color.y, particles[i].color.z, particles[i].life));
//                   // glPushAttrib(GL_CURRENT_BIT);
//                 //   glTranslatef(translate.x, translate.y, translate.z);
//                 //   glColor4f(particles[i].color.x,particles[i].color.y, particles[i].color.z, particles[i].life);
//                   /* glBegin(GL_POINTS);


//                    glPointSize(1000);
//                    glVertex3f(particles[i].pos.x, particles[i].pos.y, particles[i].pos.z);*/

//                    glTranslatef(translate.x, translate.y, translate.z);
//                    glPushAttrib(GL_CURRENT_BIT);

//                    glColor4f(particles[i].color.x,particles[i].color.y, particles[i].color.z, particles[i].life);
//                    renderTexturedQuad(1, 1);
//                    glPopAttrib();

//                   // glEnd();
//                  //  glPopAttrib();
//                    glPopMatrix();
//                }

//            }
//           // m_emitter->m_numParticles[j] -= (int) 1;
//            glDisable(GL_BLEND);
//            glDepthMask(true);
//            glAccum(GL_MULT,.95);
//            glAccum(GL_ACCUM, .05);
//            glAccum(GL_RETURN, 1);

//        }
//    }
//}

void View::renderAsteroids(){
    glEnable(GL_LIGHTING);
    // Set up global (ambient) lighting
    GLfloat global_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

    // Set up GL_LIGHT0 with a position and lighting properties
    GLfloat ambientLight[] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0, 1.0f };
    GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat position[] = { 0.0f, 0.0f, 1000.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    foreach (Asteroid * a, m_asteroids) {
       m_shaderPrograms["displacement"]->setUniformValueArray("p", a->m_displacement, 512);
        a->draw(m_fps, m_increment);
    }
    glDisable(GL_LIGHTING);

}


/**
  Called when the mouse wheel is turned.  Zooms the camera in and out.
**/
void View::wheelEvent(QWheelEvent *event)
{
    if (event->orientation() == Qt::Vertical)
    {
        m_camera.mouseWheel(event->delta());
    }
}





/**
  Draws text for the FPS and screenshot prompt
 **/
void View::paintText()
{
    glColor3f(1.f, 1.f, 1.f);

    // Combine the previous and current framerate
    if (m_fps >= 0 && m_fps < 1000)
    {
       m_prevFps *= 0.95f;
       m_prevFps += m_fps * 0.05f;
    }

    // QGLWidget's renderText takes xy coordinates, a string, and a font
    renderText(10, 20, "FPS: " + QString::number((int) (m_prevFps)), m_font);
    renderText(10, 35, "S: Save screenshot", m_font);
}
