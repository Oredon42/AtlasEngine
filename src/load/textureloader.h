#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

class Material;
class Database;

class TextureLoader
{
public:
    TextureLoader();

    void load(Database *pDatabase, const aiMaterial *pAiMaterial, const aiTextureType &textureType, const std::string &sTypeName, const std::string &sPath, std::vector<Texture *> &textures_loaded);
};

#endif // TEXTURELOADER_H
