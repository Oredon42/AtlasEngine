#ifndef LIGHTLOADER_H
#define LIGHTLOADER_H

class Database;
class Scene;
struct aiScene;

class LightLoader
{
public:
    LightLoader();

    void load(Database *pDatabase, const aiScene *pAiScene);
};

#endif // LIGHTLOADER_H
