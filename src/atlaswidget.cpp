#include <iostream>
#include <fstream>
#include <QFileDialog>

#include "include/atlaswidget.h"
#include "include/renderer.h"

AtlasWidget::AtlasWidget(QWidget * parent) :
    QOpenGLWidget(parent),
    m_yaw(0.0f),
    m_pitch(0.0f),
    angle(0),
    m_last_frame(0.0),
    m_window_width(window()->width()),
    m_window_height(window()->height()),
    m_fullscreen(false),
    m_num_scenes(0),
    m_current_scene(0)
{
    setFocus();

    //  Curseur
    setMouseTracking(true);

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer.start(41);
    m_time.start();

    for(GLuint i = 0; i < 1024; ++i)
        m_keys[i] = false;
}

AtlasWidget::~AtlasWidget()
{

}

void AtlasWidget::initializeGL()
{
#if defined(_WIN32)
    glewExperimental = GL_TRUE;
    glewInit();
#endif

    makeCurrent();

    /*  SCENES MODIFICATION */

    addScene();

    /*GLuint nb_boucles = 6;
    for(GLuint i = 0; i < nb_boucles; ++i)
        for(GLuint j = 0; j < nb_boucles; ++j)
            for(GLuint k = 0; k < nb_boucles; ++k)
            {
                m_current_scene->importFile("/obj/cube2/cube2.obj");
                m_current_scene->translate(glm::vec3(2*i,j,k),"Cube");
            }*/

    //m_current_scene->importFile("/obj/cube2/cube2.obj");
    //m_current_scene->importFile("/obj/Astroboy/astroBoy_walk_Maya2.dae");
    //m_current_scene->importFile("/obj/bras/bras2.dae");
    //m_current_scene->importFile("/obj/astro/astro.fbx");
    //m_current_scene->importAnimation("default", "/obj/astro/astro_pose1.fbx");
    //m_current_scene->importFile("/obj/dabrovic-sponza/sponza.obj");
    m_current_scene->importFile("/obj/dragon/dragon.obj");
    //m_current_scene->importFile("/obj/Models/3spheres.dae");
    //m_current_scene->importFile("/obj/Sponza/sponza2.dae");
    //m_current_scene->importFile("/obj/SimpleModel/demo.dae");


    m_current_scene->addPointLight(glm::vec3(3.f), glm::vec3(0.2f), glm::vec3(0.8f), glm::vec3(1.0f), 1.f, 0.09f, 0.032f);

    /*  END OF SCENES MODIFICATION */

    m_renderer.init(m_path, m_window_width, m_window_height, m_current_scene->numberOfDirights(), m_current_scene->numberOfPointLights(), m_current_scene->numberOfSpotLights());

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_FILL);
}

void AtlasWidget::resizeGL(int w, int h)
{
    m_window_height = h;
    m_window_width =  w;

    m_mouse_last_x = w * 0.5;
    m_mouse_last_y = h * 0.5;

    QPoint glob = mapToGlobal(QPoint(m_mouse_last_x, m_mouse_last_y));
    QCursor::setPos(glob);
}

void AtlasWidget::paintGL()
{
    GLuint current_time = m_time.elapsed();
    m_render_time = current_time;

    //m_renderer.drawSceneDeffered(*m_current_scene, (float)(current_time - m_last_frame), m_window_width, m_window_height, m_keys);
    m_renderer.drawSceneForward(*m_current_scene, (GLfloat)(current_time - m_last_frame), m_window_width, m_window_height, m_keys);

    m_last_frame = current_time;
}

void AtlasWidget::keyPressEvent(QKeyEvent * e)
{
    switch(e->key())
    {
    case Qt::Key_Up:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;

    case Qt::Key_Down:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;

    case Qt::Key_Right:
        //m_current_scene = (++m_current_scene == m_num_scenes)?0:m_current_scene;
        break;

    case Qt::Key_Left:
        //m_current_scene = (m_current_scene == 0)?m_current_scene:m_num_scenes - 1;
        break;

    case Qt::Key_F:
        /*if(m_fullscreen)
        {
            m_window_width = 800;
            m_window_height = 600;
            setWindowState(Qt::WindowMaximized);
            setGeometry(0, 0, 800, 600);
            m_fullscreen = false;
        }
        else
        {
            setWindowState(Qt::WindowFullScreen);
            m_window_width = QApplication::desktop()->width();
            m_window_height = QApplication::desktop()->height();
            glViewport(0, 0, m_window_width, m_window_height);
            m_fullscreen = true;
        }*/
        break;

    default:
        if(e->key() >= 0 && e->key() < 1024)
            m_keys[e->key()] = true;
        break;
    }
}

void AtlasWidget::keyReleaseEvent(QKeyEvent * e)
{
    if(e->key() == Qt::Key_Escape)
        QCoreApplication::instance()->quit();
    else if(e->key() >= 0 && e->key() < 1024)
        m_keys[e->key()] = false;
}

void AtlasWidget::mouseMoveEvent(QMouseEvent *e)
{
    GLfloat xoffset = QCursor::pos().x() - m_mouse_last_x;
    GLfloat yoffset = m_mouse_last_y - QCursor::pos().y();

    if(xoffset != 0 || yoffset != 0)
        m_current_scene->updateCamera(xoffset, yoffset);

     QCursor::setPos(QPoint(m_mouse_last_x, m_mouse_last_y));
}

void AtlasWidget::setPath(const std::string &path)
{
    m_path = path;

    m_path = m_path.substr(0, m_path.find_last_of('/'));
#if defined(__APPLE__) || defined(__linux__)
    m_path = m_path.substr(0, m_path.find_last_of('/'));
#endif
}

/*void AtlasWidget::reloadShaders()
{
    for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
       m_shaders[i].reload();

    m_renderer.reloadShaders();
}*/
