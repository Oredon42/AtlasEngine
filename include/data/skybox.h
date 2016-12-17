#ifndef SKYBOX_H
#define SKYBOX_H

#include "model.h"
#include "shader.h"

class Skybox : public Model
{
public:
    Skybox();
    Skybox(std::string texture_path, std::string path);
    virtual ~Skybox(){}

    void init(std::string texture_path, std::string path);
    void Draw(glm::mat4 view);

    Shader getShader() const;
    GLboolean isInitialised() const;

private:
    GLboolean m_initialised;
};

#endif // SKYBOX_H
