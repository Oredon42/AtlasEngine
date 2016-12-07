#include "include/loader/fileloader.h"
#include "include/data/scene.h"

FileLoader::FileLoader()
{

}

void FileLoader::load(Scene scene)
{
    /*std::string new_path = m_path + path;
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

    m_meshloader.load(scene);
    m_armatureloader.load();
    m_animationloader.load();

    m_roots.push_back(new SceneGraphRoot(scene, global_inverse_transform, new_path, m_models, m_render_time));
    m_roots[m_roots.size() - 1]->spreadTransform(glm::mat4(1));

*/
}

void FileLoader::loadLights()
{
    /*if(scene->HasLights())
    {
        for(GLuint i = 0; i < scene->mNumLights; ++i)
        {
            const aiLight *current_light = scene->mLights[i];
            switch(current_light->mType)
            {
            case aiLightSource_DIRECTIONAL :
                m_dirlights.push_back(DirLight(glm::vec3(current_light->mDirection.x, current_light->mDirection.y, current_light->mDirection.z),
                                               glm::vec3(current_light->mColorAmbient.r, current_light->mColorAmbient.g, current_light->mColorAmbient.b),
                                               glm::vec3(current_light->mColorDiffuse.r, current_light->mColorDiffuse.g, current_light->mColorDiffuse.b),
                                               glm::vec3(current_light->mColorSpecular.r, current_light->mColorSpecular.g, current_light->mColorSpecular.b)));
                break;

            case aiLightSource_POINT :
            {
                //  Get the node transform matrix
                aiNode *node = scene->mRootNode->FindNode(scene->mLights[i]->mName);
                glm::mat4 glm_transform;
                for(GLuint i = 0; i < 4; ++i)
                    for(GLuint j = 0; j < 4; ++j)
                        glm_transform[j][i] = node->mTransformation[i][j];

                //  Apply transformation
                glm::vec4 position = glm_transform * glm::vec4(current_light->mPosition.x, current_light->mPosition.y, current_light->mPosition.z, 1.f);
                if(current_light->mAttenuationConstant == 0 && current_light->mAttenuationLinear == 0 && current_light->mAttenuationQuadratic == 0)
                    m_pointlights.push_back(PointLight(m_pointlights.size(), glm::vec3(position.x / position.w, position.y / position.w, position.z / position.w),
                                                       glm::vec3(current_light->mColorDiffuse.r, current_light->mColorDiffuse.g, current_light->mColorDiffuse.b)));
                else
                    m_pointlights.push_back(PointLight(m_pointlights.size(), glm::vec3(position.x / position.w, position.y / position.w, position.z / position.w),
                            glm::vec3(current_light->mColorDiffuse.r, current_light->mColorDiffuse.g, current_light->mColorDiffuse.b),
                            current_light->mAttenuationConstant,
                            current_light->mAttenuationLinear,
                            current_light->mAttenuationQuadratic));
            }
                break;

            case aiLightSource_SPOT :
                m_spotlights.push_back(SpotLight(glm::vec3(current_light->mPosition.x, current_light->mPosition.y, current_light->mPosition.z),
                                                 glm::vec3(current_light->mDirection.x, current_light->mDirection.y, current_light->mDirection.z),
                                                 current_light->mAngleInnerCone,
                                                 current_light->mAngleOuterCone,
                                                 glm::vec3(current_light->mColorAmbient.r, current_light->mColorAmbient.g, current_light->mColorAmbient.b),
                                                 glm::vec3(current_light->mColorDiffuse.r, current_light->mColorDiffuse.g, current_light->mColorDiffuse.b),
                                                 glm::vec3(current_light->mColorSpecular.r, current_light->mColorSpecular.g, current_light->mColorSpecular.b),
                                                 current_light->mAttenuationConstant,
                                                 current_light->mAttenuationLinear,
                                                 current_light->mAttenuationQuadratic));
                break;

            default:
                break;
            }
        }
    }*/
}

void FileLoader::loadCameras()
{
    /*if(scene->HasCameras())
    {
        for(GLuint i = 0; i < scene->mNumCameras; ++i)
            m_cameras.push_back(new Camera(glm::vec3(scene->mCameras[i]->mPosition.x, scene->mCameras[i]->mPosition.y, scene->mCameras[i]->mPosition.z),
                                           glm::vec3(scene->mCameras[i]->mLookAt.x,scene->mCameras[i]->mLookAt.y, scene->mCameras[i]->mLookAt.z),
                                           glm::vec3(scene->mCameras[i]->mUp.x, scene->mCameras[i]->mUp.y, scene->mCameras[i]->mUp.z),
                                           0.005f,
                                           scene->mCameras[i]->mHorizontalFOV));
    }*/
}
