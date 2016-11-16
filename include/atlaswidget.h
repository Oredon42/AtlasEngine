#ifndef RENDERINGWIDGET_H
#define RENDERINGWIDGET_H

#include <QtCore/QCoreApplication>
#include <QtGui/QKeyEvent>
#include <QtCore/qmath.h>
#include <QtCore/QTime>
#include <QtCore/QTimer>

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

#include <QOpenGLWidget>

#include "renderer.h"
#include "scene.h"

class AtlasWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    AtlasWidget(QWidget * parent = 0);
    ~AtlasWidget();
    void setPath(const std::string &path);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);

private:
    GLfloat m_yaw;
    GLfloat m_pitch;
    GLfloat m_mouse_saved_x;
    GLfloat m_mouse_saved_y;
    GLfloat m_mouse_last_x;
    GLfloat m_mouse_last_y;
    QTime m_time;
    GLboolean m_keys[1024];
    std::string m_path;
    GLfloat angle;
    GLuint m_last_frame;
    GLuint m_window_width;
    GLuint m_window_height;
    GLboolean m_fullscreen;
    QTimer m_timer;
    GLuint m_num_scenes;
    std::vector<Scene> m_scenes;
    Renderer m_renderer;
    GLfloat m_render_time;
    Scene *m_current_scene;

    inline void addScene(){m_scenes.push_back(Scene(m_path, m_render_time)); m_current_scene = &m_scenes[m_num_scenes++];}
};

#endif // RENDERINGWIDGET_H
