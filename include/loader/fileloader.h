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

    GLboolean load(const std::string path, Scene *scene, MaterialLibrary &material_library, int flags = aeNoFlag);
    GLboolean load(const std::string path, SceneGraphNode *scene_graph_node, MaterialLibrary &material_library, int flags = aeNoFlag);
    void loadLights(const aiScene *ai_scene, Scene *scene);
    void loadCameras(const aiScene *ai_scene, Scene *scene);
    void loadMaterials(const aiScene *ai_scene, MaterialLibrary &material_library, const std::string &path);

private:
    void processFlags(const int &flags);

    MeshLoader m_meshloader;
    AnimationLoader m_animationloader;

    GLboolean m_process_meshes;
    GLboolean m_process_lights;
    GLboolean m_process_cameras;
    GLboolean m_process_armatures;
    GLboolean m_process_animations;

};

#endif // FILELOADER_H
