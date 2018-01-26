#include "src/loader/lightloader.h"
#include "src/core/data/database.h"
#include "src/core/data/scene.h"
#include "src/core/data/light.h"
#include "assimp/scene.h"

#include "assimp/scene.h"

LightLoader::LightLoader()
{

}

void LightLoader::load(Database *pDatabase, const aiScene *pAiScene)
{
    if(pAiScene->HasLights() == true)
    {
        for(unsigned int uiLightIndex = 0; uiLightIndex < pAiScene->mNumLights; ++uiLightIndex)
        {
            const aiLight *pCurrentAiLight = pAiScene->mLights[uiLightIndex];
            const std::string strLightName = pCurrentAiLight->mName.C_Str();

            switch(pCurrentAiLight->mType)
            {
            case aiLightSource_POINT :
            {
                //  Get the node transform matrix
                const aiNode *pNode = pAiScene->mRootNode->FindNode(pAiScene->mLights[uiLightIndex]->mName);
                const glm::mat4 mTransformation = assimpToGlmMat4(node->mTransformation);

                //  Apply transformation
                const glm::vec4 vPosition = mTransformation * glm::vec4(pCurrentAiLight->mPosition.x, pCurrentAiLight->mPosition.y, pCurrentAiLight->mPosition.z, 1.f);
                const glm::vec3 vNormalizedPosition = glm::vec3(vPosition.x, vPosition.y, vPosition.z) / vPosition.w;
                const glm::vec3 vColor = glm::vec3(current_light->mColorDiffuse.r, current_light->mColorDiffuse.g, current_light->mColorDiffuse.b);
                const float fIntensity = 1.f;

                PointLight *pPointlight = new PointLight(vColor, fIntensity, vNormalizedPosition);

                pDatabase->insertPointlight(pPointlight, strLightName);
            }
                break;

            default:
                break;
            }
        }
    }
}
