#ifndef SKYBOX_H
#define SKYBOX_H

#include "core/data/model.h"

class Skybox : public Model
{
public:
    Skybox();
    Skybox(std::string texture_path, std::string path);
    virtual ~Skybox(){}

    virtual inline void draw() const;
};

#endif // SKYBOX_H
