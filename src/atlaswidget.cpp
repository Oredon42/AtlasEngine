#include <fstream>
#include <QFileDialog>
#include <QApplication>
#include <QDesktopWidget>

#include "include/atlaswidget.h"
#include "include/render/renderer.h"

AtlasWidget::AtlasWidget(QWidget * parent) :
    QOpenGLWidget(parent),
    m_menu(this, Qt::FramelessWindowHint),
    m_paused(false),
    m_yaw(0.0f),
    m_pitch(0.0f),
    angle(0),
    m_last_frame(0.0),
    m_fullscreen(false),
    m_num_scenes(0),
    m_current_scene(0)
{
    setFocus();

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

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    /*  SCENES MODIFICATION */

    addScene();

    //m_current_scene->importFile("/obj/cube2/cube2.obj");
    //m_current_scene->importFile("/obj/Astroboy/astroBoy_walk_Maya2.dae");
    //m_current_scene->importFile("/obj/bras/bras2.dae");
    //m_current_scene->importFile("/obj/astro/astro.fbx");
    //m_current_scene->importAnimation("default", "/obj/astro/astro_pose1.fbx");
    //m_current_scene->importFile("/obj/dabrovic-sponza/sponza.obj");
    //m_current_scene->importFile("/obj/dragon/dragon.obj");
    //m_current_scene->importFile("/obj/Models/3spheres.dae");
    //m_current_scene->importFile("/obj/Sponza/sponza2.dae");
    //m_current_scene->importFile("/obj/SimpleModel/demo.dae");
    m_file_loader.load("/obj/test/test2.dae", m_current_scene);
    //m_current_scene->importFile("/obj/test/test.dae");

    m_current_scene->rotate(glm::vec3(0, 0, 270), "Scene");

    m_current_scene->addPointLight(glm::vec3(3.f), glm::vec3(1.0f), 1.f);
    m_current_scene->addPointLight(glm::vec3(3.f,3.f,-3.f), glm::vec3(1.0f), 1.f);
    m_current_scene->addPointLight(glm::vec3(-3.f), glm::vec3(1.0f), 1.f);
    //m_current_scene->addDirLight(glm::normalize(glm::vec3(-1.f, -1.f, -1.f)),glm::vec3(0.2),glm::vec3(0.8),glm::vec3(1));
    //m_current_scene->addSpotLight(glm::vec3(0,3,0),glm::vec3(0,0,-1),glm::cos(glm::radians(12.5f)),glm::cos(glm::radians(15.0f)),glm::vec3(0.1f),glm::vec3(0.5),glm::vec3(1.f),1.f,0.7f,1.8f);

    m_current_scene->addCamera(new Camera());

    /*  END OF SCENES MODIFICATION */

    m_renderer.init(m_path, window()->width(), window()->height(), m_current_scene->numberOfDirights(), m_current_scene->numberOfPointLights(), m_current_scene->numberOfSpotLights());

    //m_renderer.resize(800, 600);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_FILL);
}

void AtlasWidget::resizeGL(int w, int h)
{
    m_renderer.setDimensions(w, h);

    m_mouse_last_x = w * 0.5;
    m_mouse_last_y = h * 0.5;

    QPoint glob = mapToGlobal(QPoint(m_mouse_last_x, m_mouse_last_y));
    QCursor::setPos(glob);
}

void AtlasWidget::paintGL()
{
    GLuint current_time = m_time.elapsed();
    m_render_time = current_time;

    m_renderer.drawScene(*m_current_scene, (float)(current_time - m_last_frame), m_keys);

    m_last_frame = current_time;
}

void AtlasWidget::keyPressEvent(QKeyEvent * e)
{
    switch(e->key())
    {
    case Qt::Key_Escape:
        pause();
        break;

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

    case Qt::Key_R:
        m_renderer.reloadShaders();
        break;

    case Qt::Key_F:
        if(m_fullscreen)
        {
            m_renderer.setDimensions(800, 600);
            parentWidget()->parentWidget()->setWindowState(Qt::WindowMaximized);
            resize(800, 600);
            setGeometry(0, 0, 800, 600);
        }
        else
        {
            parentWidget()->parentWidget()->setWindowState(Qt::WindowFullScreen);
            GLuint width = QApplication::desktop()->width(),
                   height = QApplication::desktop()->height();
            m_renderer.setDimensions(width, height);
            resize(width, height);
            glViewport(0, 0, width, height);
        }
        m_fullscreen = !m_fullscreen;
        break;

    case Qt::Key_H:
        m_renderer.switchHDR();
        break;

    case Qt::Key_B:
        m_renderer.switchBloom();
        break;

    case Qt::Key_N:
        m_renderer.switchAdaptation();
        break;

    default:
        if(e->key() >= 0 && e->key() < 1024)
            m_keys[e->key()] = true;
        break;
    }
}

void AtlasWidget::keyReleaseEvent(QKeyEvent * e)
{
    /*if(e->key() == Qt::Key_Escape)
        QCoreApplication::instance()->quit();
    else if(e->key() >= 0 && e->key() < 1024)*/
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

void AtlasWidget::unPause()
{
    if(m_paused == true)
    {
        m_paused = false;

        QApplication::setOverrideCursor(Qt::BlankCursor);
    }
}

void AtlasWidget::pause()
{
    if(m_paused == false)
    {
        m_paused = true;

        QApplication::setOverrideCursor(Qt::ArrowCursor);
        m_menu.exec();
    }
}
