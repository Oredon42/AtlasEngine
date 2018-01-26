#ifndef RENDERING_QUAD_H
#define RENDERING_QUAD_H

#include "core/model.h"

class RenderingQuad : public Model
{
public:
    RenderingQuad(const GLfloat &fSize);

    virtual inline void draw() const;
    virtual void setupBuffers();

private:
    GLfloat m_fSize;
};

#endif // RENDERING_QUAD_H
