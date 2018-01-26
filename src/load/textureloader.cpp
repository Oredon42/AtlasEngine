#include "src/loader/textureloader.h"

#include "assimp/scene.h"

TextureLoader::TextureLoader()
{

}

void TextureLoader::load(const aiMaterial *ai_material, const aiTextureType &type, const std::string &type_name, const std::string &path, std::vector<Texture *> &textures_loaded)
{
    std::vector<Texture *> textures;

    //  For each texture
    for(unsigned int i = 0; i < ai_material->GetTextureCount(type); ++i)
    {
        aiString ai_texture_path;
        ai_material->GetTexture(type, i, &ai_texture_path);
        GLboolean skip = GL_FALSE;

        //  Check if texture is not already loaded
        for(size_t j = 0; j < textures_loaded.size(); ++j)
        {
            if(textures_loaded[j]->getPath() == ai_texture_path.C_Str())
            {
                //  If texture already loaded, push it back
                textures.push_back(textures_loaded[j]);
                skip = GL_TRUE;
                break;
            }
        }
        if(!skip)
        {
            //  If not already loaded, load it

            //  Concatenate directory path to filename
            std::string strFilePath;
            if(acPath[0] == '/')
            {
                strFilePath = acPath;
            }
            else
            {
                strFilePath = strTexturePath + '/' + std::string(acPath);
            }

            Texture *texture = new Texture();
            texture->generateFromFile(strFilePath, type_name);
            textures.push_back(texture);
            textures_loaded.push_back(textures[textures.size()-1]);
        }
    }
    return textures;
}
}
