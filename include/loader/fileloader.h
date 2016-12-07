#ifndef FILELOADER_H
#define FILELOADER_H

#include <string>

#include "lib/assimp/Importer.hpp"
#include "lib/assimp/scene.h"
#include "lib/assimp/postprocess.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

#include "meshloader.h"
#include "armatureloader.h"
#include "animationloader.h"

class Scene;

class FileLoader
{
public:
    FileLoader();

    void load(Scene scene);
    void loadLights();
    void loadCameras();

private:
    MeshLoader m_meshloader;
    ArmatureLoader m_armatureloader;
    AnimationLoader m_animationloader;

};

#endif // FILELOADER_H
