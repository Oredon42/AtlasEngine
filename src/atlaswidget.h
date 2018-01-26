#ifndef RENDERINGWIDGET_H
#define RENDERINGWIDGET_H

#include "src/core/render/renderer.h"
#include "src/core/data/scene.h"
#include "src/loader/fileloader.h"
#include "src/geometry/geometrytransform.h"
#include "src/core/data/database.h"

#include <QOpenGLWidget>
#include <QtCore/QElapsedTimer>
#include <QtCore/QTimer>

class QKeyEvent;

class AtlasWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    AtlasWidget(QWidget * parent = 0);
    ~AtlasWidget();

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);

protected slots:
    void unPause();

private:
    inline void addScene(){m_scenes.push_back(new Scene(m_path, m_render_time)); m_current_scene = m_scenes[++m_current_scene_index];}
    void setCurrentPipeline(const std::string &pipeline_name);
    void pause();

    void createRenderScene();
    void createGeometryScene();

    GLboolean m_paused;

    GLfloat m_mouse_saved_x;
    GLfloat m_mouse_saved_y;
    GLfloat m_mouse_last_x;
    GLfloat m_mouse_last_y;
    QElapsedTimer m_time;
    GLboolean m_keys[1024];
    std::string m_path;
    GLuint m_last_frame;
    GLuint m_window_width;
    GLuint m_window_height;
    GLboolean m_fullscreen;
    QTimer m_timer;
    std::vector<Scene *> m_scenes;
    Renderer m_renderer;
    GLfloat m_render_time;
    Scene *m_current_scene;
    GLuint m_current_scene_index;

    FileLoader m_file_loader;
    GeometryTransform m_geometry_process;
    Database m_database;
};

#endif // RENDERINGWIDGET_H
