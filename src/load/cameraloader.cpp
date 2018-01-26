#include "src/loader/cameraloader.h"
#include "src/core/data/scene.h"
#include "src/core/data/camera.h"
#include "src/core/data/database.h"

#include "assimp/scene.h"

CameraLoader::CameraLoader()
{

}

/*
 * Load cameras from a aiScene
 * and eventually insert it inside Scene
 * */
void CameraLoader::load(Database *pDatabase, const aiScene *pAiScene, Scene *pScene)
{
    if(pAiScene->HasCameras() == true)
    {
        //  Loop on aiScene cameras
        for(unsigned int uiCameraIndex = 0; uiCameraIndex < pAiScene->mNumCameras; ++uiCameraIndex)
        {
            const glm::vec3 vPosition = glm::vec3(pAiScene->mCameras[i]->mPosition.x, pAiScene->mCameras[i]->mPosition.y, pAiScene->mCameras[i]->mPosition.z);
            const glm::vec3 vFront = glm::vec3(pAiScene->mCameras[i]->mLookAt.x, pAiScene->mCameras[i]->mLookAt.y, pAiScene->mCameras[i]->mLookAt.z);
            const glm::vec3 vUp = glm::vec3(pAiScene->mCameras[i]->mUp.x, pAiScene->mCameras[i]->mUp.y, pAiScene->mCameras[i]->mUp.z);
            const float fSpeed = 0.005f;
            const float fFov = pAiScene->mCameras[i]->mHorizontalFOV;

            Camera *pCurrentCamera = new Camera(vPosition, vFront, vUp, fSpeed, fFov);

            const std::string strCameraName = pAiScene->mCameras[i]->mName.C_Str();

            pDatabase->insertCamera(pCurrentCamera, strCameraName);

            if(pScene != nullptr)
            {
                pScene->addCamera(pCurrentCamera);
            }
        }
    }
}
