#ifndef FXAAPROCESS_H
#define FXAAPROCESS_H

#include "renderprocess.h"

class FXAARenderProcess : public RenderProcess
{
public:
    FXAARenderProcess(const GLuint &width, const GLuint &height);
};

#endif // FXAAPROCESS_H
