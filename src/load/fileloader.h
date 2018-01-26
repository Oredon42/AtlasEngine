#ifndef FILELOADER_H
#define FILELOADER_H

#include <string>

class Scene;
class Database;
class AnimationLoader;
class CameraLoader;
class LightLoader;
class MaterialLoader;
class ModelLoader;
class TextureLoader;

class FileLoader
{
public:
    FileLoader();
    ~FileLoader();

    void load(Database *pDatabase, const std::string &strPath);

private:
    AnimationLoader *m_pAnimationLoader;
    CameraLoader    *m_pCameraLoader;
    LightLoader     *m_pLightLoader;
    MaterialLoader  *m_pMaterialLoader;
    ModelLoader     *m_pModelLoader;
    TextureLoader   *m_pTextureLoader;
};

#endif // FILELOADER_H
