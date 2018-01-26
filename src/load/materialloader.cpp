#include "src/loader/materialloader.h"
#include "src/loader/textureloader.h"

#include "assimp/scene.h"

#include <strstream>

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

MaterialLoader::MaterialLoader()
{

}

void MaterialLoader::load(Database *pDatabase, const aiScene *pAiScene, const std::string &strPath)
{
    if(aiScene.HasMaterials() == true)
    {
        for(unsigned int uiMaterialIndex = 0; uiMaterialIndex < pAiScene->mNumMaterials; ++uiMaterialIndex)
        {
            const aiMaterial *pAiMaterial = pAiScene->mMaterials[uiMaterialIndex];

            const void *pAddress = static_cast<const void *>(pAiMaterial);
            std::stringstream ss;
            ss << address;
            const std::string strName = ss.str();

            //  Attributes
            const float fRoughness = 1.f;
            float fRefraction;
            float fOpacity;

            aiColor3D aiColor;
            pAiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
            const glm::vec3 vColor = glm::vec3(aiColor.r, aiColor.g, aiColor.b);
            pAiMaterial->Get(AI_MATKEY_OPACITY, fOpacity);
            pAiMaterial->Get(AI_MATKEY_REFRACTI, fRefraction);

            m_pTextureLoader->load();

            //  Textures
            std::vector<Texture *> vDiffuseMaps = loadMaterialTextures(ai_material, aiTextureType_DIFFUSE, "texture_diffuse", strPath, textures_loaded);
            m_textures.insert(m_textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            std::vector<Texture *> vSpecularMaps = loadMaterialTextures(ai_material, aiTextureType_SPECULAR, "texture_specular", strPath, textures_loaded);
            m_textures.insert(m_textures.end(), specularMaps.begin(), specularMaps.end());
            std::vector<Texture *> vNormalMaps = loadMaterialTextures(ai_material, aiTextureType_NORMALS, "texture_normal", strPath, textures_loaded);
            m_textures.insert(m_textures.end(), normalMaps.begin(), normalMaps.end());

            m_shader_type = getShaderType(animated, diffuseMaps.size(), specularMaps.size(), normalMaps.size());

            //  Normal map check
            m_has_normal_map = GL_FALSE;
            if(normalMaps.size() > 0)
                m_has_normal_map = GL_TRUE;
            //database.addMaterial(ai_material, name, GL_FALSE, path);
        }
    }
}
