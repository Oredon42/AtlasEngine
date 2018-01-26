#include "src/loader/fileloader.h"
#include "src/core/data/database.h"
#include "src/core/data/scene.h"
#include "src/loader/animationloader.h"
#include "src/loader/armatureloader.h"
#include "src/loader/cameraloader.h"
#include "src/loader/lightloader.h"
#include "src/loader/materialloader.h"
#include "src/loader/modelloader.h"
#include "src/loader/textureloader.h"

#include "assimp/scene.h"
#include "assimp/Importer.hpp"

FileLoader::FileLoader()
{
    m_pAnimationLoader = new AnimationLoader();
    m_pCameraLoader = new CameraLoader();
    m_pLightLoader = new LightLoader();
    m_pMaterialLoader = new MaterialLoader();
    m_pModelLoader = new ModelLoader();
    m_pTextureLoader = new TextureLoader();
}

FileLoader::~FileLoader()
{
    delete m_pAnimationLoader;
    delete m_pCameraLoader;
    delete m_pLightLoader;
    delete m_pMaterialLoader;
    delete m_pModelLoader;
    delete m_pTextureLoader;
}

void FileLoader::load(Database *pDatabase, const std::string &strPath)
{
    std::string strNewPath = scene->getPath() + strNewPath;
    Assimp::Importer importer;
    const aiScene* pAiScene = importer.ReadFile(strNewPath,   aiProcess_Triangulate |
                                                            aiProcess_ImproveCacheLocality |
                                                            aiProcess_OptimizeMeshes |
                                                            aiProcess_FlipUVs |
                                                            aiProcess_CalcTangentSpace);

    m_pModelLoader;
    m_pAnimationLoader;
    m_pCameraLoader;
    m_pLightLoader;
    m_pMaterialLoader;
    m_pTextureLoader;
}
