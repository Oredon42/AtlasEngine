#include "include/loader/fileloader.h"
#include "include/data/scene.h"

FileLoader::FileLoader() :
    m_process_meshes(GL_TRUE),
    m_process_lights(GL_TRUE),
    m_process_cameras(GL_TRUE),
    m_process_armatures(GL_TRUE),
    m_process_animations(GL_TRUE)
{

}

GLboolean FileLoader::load(const std::string path, Scene *scene, int flags)
{
    processFlags(flags);

    std::string new_path = scene->getPath() + path;
    Assimp::Importer importer;
    const aiScene* ai_scene = importer.ReadFile(new_path,  aiProcess_Triangulate |
                                                            aiProcess_ImproveCacheLocality |
                                                            aiProcess_OptimizeMeshes |
                                                            aiProcess_FlipUVs |
                                                            aiProcess_CalcTangentSpace);

    if(!ai_scene || ai_scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return GL_FALSE;
    }
    new_path = new_path.substr(0, new_path.find_last_of('/'));

    if(m_process_meshes)
        m_meshloader.load(ai_scene, scene, m_process_armatures);
    if(m_process_animations)
        m_animationloader.load(ai_scene, scene);
    if(m_process_lights)
        loadLights(ai_scene, scene);
    if(m_process_cameras)
        loadCameras(ai_scene, scene);

    scene->getLastRoot()->spreadTransform(glm::mat4(1.f));

    return GL_TRUE;
}

void FileLoader::loadLights(const aiScene *ai_scene, Scene *scene)
{
    if(ai_scene->HasLights())
    {
        for(GLuint i = 0; i < ai_scene->mNumLights; ++i)
        {
            const aiLight *current_light = ai_scene->mLights[i];
            switch(current_light->mType)
            {

            case aiLightSource_POINT :
            {
                //  Get the node transform matrix
                aiNode *node = ai_scene->mRootNode->FindNode(ai_scene->mLights[i]->mName);
                glm::mat4 glm_transform = assimpToGlmMat4(node->mTransformation);

                //  Apply transformation
                glm::vec4 position = glm_transform * glm::vec4(current_light->mPosition.x, current_light->mPosition.y, current_light->mPosition.z, 1.f);


                scene->addPointLight(PointLight(scene->numberOfPointLights(), glm::vec3(position.x / position.w, position.y / position.w, position.z / position.w),
                                               glm::vec3(current_light->mColorDiffuse.r, current_light->mColorDiffuse.g, current_light->mColorDiffuse.b), 1.f));
            }
                break;

            default:
                break;
            }
        }
    }
}

void FileLoader::loadCameras(const aiScene *ai_scene, Scene *scene)
{    
    if(ai_scene->HasCameras())
    {
        for(GLuint i = 0; i < ai_scene->mNumCameras; ++i)
            scene->addCamera(new Camera(glm::vec3(ai_scene->mCameras[i]->mPosition.x, ai_scene->mCameras[i]->mPosition.y, ai_scene->mCameras[i]->mPosition.z),
                                        glm::vec3(ai_scene->mCameras[i]->mLookAt.x, ai_scene->mCameras[i]->mLookAt.y, ai_scene->mCameras[i]->mLookAt.z),
                                        glm::vec3(ai_scene->mCameras[i]->mUp.x, ai_scene->mCameras[i]->mUp.y, ai_scene->mCameras[i]->mUp.z),
                                        0.005f,
                                        ai_scene->mCameras[i]->mHorizontalFOV));
    }
}

void FileLoader::processFlags(const int &flags)
{
    m_process_meshes = (flags&aeMissMesh)?GL_FALSE:GL_TRUE;
    m_process_lights = (flags&aeMissLight)?GL_FALSE:GL_TRUE;
    m_process_cameras = (flags&aeMissCamera)?GL_FALSE:GL_TRUE;
    m_process_armatures = (flags&aeMissArmature)?GL_FALSE:GL_TRUE;
    m_process_animations = (flags&aeMissAnimation)?GL_FALSE:GL_TRUE;
}
