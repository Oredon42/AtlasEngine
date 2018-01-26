#ifndef MATERIALLOADER_H
#define MATERIALLOADER_H

#include <vector>

class Material;
class Database;
class TextureLoader;
struct aiScene;

class MaterialLoader
{
public:
    MaterialLoader();

    void load(Database *pDatabase, const aiScene *pAiScene, const std::string &strPath);

private:
    TextureLoader *m_pTextureLoader;
};

#endif // MATERIALLOADER_H
