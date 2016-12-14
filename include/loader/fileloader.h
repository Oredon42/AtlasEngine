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
#include "animationloader.h"

#include "openglincludes.h"

enum Flags
{
    aeNoFlag = 0x0,
    aeMissMesh = 0x1,
    aeMissLight = 0x2,
    aeMissCamera = 0x4,
    aeMissArmature = 0x8,
    aeMissAnimation = 0x16
};

class Scene;

class FileLoader
{
public:
    FileLoader();

    GLboolean load(const std::string path, Scene *scene, Flags flags = aeNoFlag);
    void loadLights(const aiScene *ai_scene, Scene *scene);
    void loadCameras(const aiScene *ai_scene, Scene *scene);

private:
    void processFlags(const Flags &flags);

    MeshLoader m_meshloader;
    AnimationLoader m_animationloader;

    GLboolean m_process_meshes;
    GLboolean m_process_lights;
    GLboolean m_process_cameras;
    GLboolean m_process_armatures;
    GLboolean m_process_animations;

};

#endif // FILELOADER_H