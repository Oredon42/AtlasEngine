#ifndef RENDERER_H
#define RENDERER_H

#include <string>
#include <vector>
#include <math.h>

#include "openglincludes.h"

#include "quad.h"
#include "shader.h"
#include "framebuffer.h"

#include "process/geometryrenderprocess.h"
#include "process/ssaorenderprocess.h"
#include "process/hdrrenderprocess.h"
#include "pipeline.h"

class Scene;

class Renderer
{
public:
    Renderer();

    void init(const std::string &path, const GLuint &width, const GLuint &height, const GLuint &nb_dirlights, const GLuint &nb_pointlights, const GLuint &nb_spotlights);
    void resize(const GLuint &width, const GLuint &height);

    void drawScene(const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024]) const;

    void reloadShaders();

    //  Setters
    inline void setDimensions(const GLuint &width, const GLuint &height) {m_width = width; m_height = height;}

private:
    void initQuad();

    GLuint m_width;
    GLuint m_height;

    Quad m_quad;

    Shader m_quad_shader;

    std::vector<Pipeline> m_pipelines;
};

#endif // RENDERER_H
