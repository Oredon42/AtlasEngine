#ifndef CAMERALOADER_H
#define CAMERALOADER_H

struct aiScene;
class Database;
class Scene;

/*
 * This class loads Cameras from an aiScene
 * And add them to Database
 * */
class CameraLoader
{
public:
    CameraLoader();

    void load(Database *pDatabase, const aiScene *pAiScene, Scene *pScene = nullptr);
};

#endif // CAMERALOADER_H
