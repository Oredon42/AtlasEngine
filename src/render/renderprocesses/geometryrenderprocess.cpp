#include "src/core/render/process/geometryrenderprocess.h"
#include "src/core/data/scene.h"
#include "src/core/data/shader.h"
#include "src/core/data/model.h"
#include "src/core/data/material.h"

/*
 * GeometryPassShaderType
 * */

GeometryRenderProcess::GeometryPassShaderType::GeometryPassShaderType(const Model *pModel) :
    uiFlags(0)
{
    const Material &pMaterial = pModel->getMaterial();

    if(pMaterial->hasAlbedoMap())
    {
        uiFlags |= AlbedoMap;
    }
    if(pMaterial->hasMetalnessMap())
    {
        uiFlags |= MetalnessMap;
    }
    if(pMaterial->hasRoughnessMap())
    {
        uiFlags |= RoughnessMap;
    }
    if(pMaterial->hasRefractionMap())
    {
        uiFlags |= RefractionMap;
    }
    if(pMaterial->hasOpacityMap())
    {
        uiFlags |= OpacityMap;
    }
    if(pMaterial->hasNormalMap())
    {
        uiFlags |= NormalMap;
    }
    if(pModel->hasArmature())
    {
        uiFlags |= Animated;
    }
}


/*
 * GeometryRenderProcess
 * */

GeometryRenderProcess::GeometryRenderProcess() :
    RenderProcess(0, 4),
    m_aShaders(nullptr)
{

}

void GeometryRenderProcess::init(const GLsizei &iWidth, const GLsizei &iHeight)
{
    RenderProcess::init(iWidth, iHeight);

    initShaders();

    m_shader_types[0].setValues(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, 0);
    m_shader_types[1].setValues(GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE, 1);
    m_shader_types[2].setValues(GL_FALSE, GL_TRUE, GL_FALSE, GL_FALSE, 2);
    m_shader_types[3].setValues(GL_TRUE, GL_TRUE, GL_FALSE, GL_FALSE, 3);
    m_shader_types[4].setValues(GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE, 4);
    m_shader_types[5].setValues(GL_TRUE, GL_FALSE, GL_TRUE, GL_FALSE, 5);
    m_shader_types[6].setValues(GL_FALSE, GL_TRUE, GL_TRUE, GL_FALSE, 6);
    m_shader_types[7].setValues(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE, 7);

    m_shader_types[8].setValues(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE, 8);
    m_shader_types[9].setValues(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE, 9);
    m_shader_types[10].setValues(GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE, 10);
    m_shader_types[11].setValues(GL_TRUE, GL_TRUE, GL_FALSE, GL_TRUE, 11);
    m_shader_types[12].setValues(GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE, 12);
    m_shader_types[13].setValues(GL_TRUE, GL_FALSE, GL_TRUE, GL_TRUE, 13);
    m_shader_types[14].setValues(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE, 14);
    m_shader_types[15].setValues(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE, 15);

    m_gBuffer.init(iWidth, iHeight);
    std::vector<FramebufferTextureDatas> gTexture_datas;
    gTexture_datas.push_back(FramebufferTextureDatas(GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE));
    gTexture_datas.push_back(FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE));
    gTexture_datas.push_back(FramebufferTextureDatas(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE));
    gTexture_datas.push_back(FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE));
    m_gBuffer.attachTextures(gTexture_datas);
    std::vector<FramebufferRenderbufferDatas> gRenderbuffer_datas;
    gRenderbuffer_datas.push_back(FramebufferRenderbufferDatas(GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT));
    m_gBuffer.attachRenderBuffers(gRenderbuffer_datas);

    // Setup shaders
    for(size_t i = 0; i < NB_SHADER_TYPES; ++i)
        m_shaders[i].initGeometryPass(m_shader_types[i]);

    m_out_textures[0] = m_gBuffer.getTexture(0);
    m_out_textures[1] = m_gBuffer.getTexture(1);
    m_out_textures[2] = m_gBuffer.getTexture(2);
    m_out_textures[3] = m_gBuffer.getTexture(3);
}

void GeometryRenderProcess::resize(const GLsizei &iWidth, const GLsizei &iHeight)
{
    RenderProcess::resize(iWidth, iHeight);

    m_gBuffer.resize(iWidth, iHeight);
}

void GeometryRenderProcess::process(const float &fRenderTime)
{
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, m_gBuffer.width(), m_gBuffer.height());
    m_gBuffer.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //  Model
    const glm::mat3 mInvertedTransposed = glm::value_ptr(glm::transpose(m_pTransform->getInverseTransform().toMat3()));
    glUniformMatrix4fv(glGetUniformLocation(pShader->getProgram(), "model"), 1, GL_FALSE, glm::value_ptr(m_pTransform->getTransform()));
    glUniformMatrix3fv(glGetUniformLocation(pShader->getProgram(), "inverted_transposed_model"), 1, GL_FALSE, );

    pShader.resetActiveTexture();

    m_pMaterial->sendDatas(pShader);

    if(m_pArmature != 0)
    {
        m_pArmature->update(pShader, m_pCurrentAnimation, fRenderTime);
    }
    //

    scene.draw(m_shaders, keys, fRenderTime, m_width, m_height);

    m_gBuffer.release();
}

Shader *GeometryRenderProcess::generateShaderFromType(const GeometryRenderProcess::GeometryPassShaderType &sShaderType)
{
    Shader *pShader = new Shader();

    /*
     * TODO: spécifier define dans fragment et/ou dans vertex etc.
     * */

    if(sShaderType.hasAlbedoMap())
    {
        v_shader_stream << "#define ALBEDO_MAP\n";
        f_shader_stream << "#define ALBEDO_MAP\n";
    }
    if(sShaderType.hasMetalnessMap())
    {
        v_shader_stream << "#define METALNESS_MAP\n";
        f_shader_stream << "#define METALNESS_MAP\n";
    }
    if(sShaderType.hasRoughnessMap())
    {
        v_shader_stream << "#define ROUGHNESS_MAP\n";
        f_shader_stream << "#define ROUGHNESS_MAP\n";
    }
    if(sShaderType.hasRefractionMap())
    {
        v_shader_stream << "#define REFRACTION_MAP\n";
        f_shader_stream << "#define REFRACTION_MAP\n";
    }
    if(sShaderType.hasOpacityMap())
    {
        v_shader_stream << "#define ALBEDO_MAP\n";
        f_shader_stream << "#define ALBEDO_MAP\n";
    }
    if(sShaderType.hasNormalMap())
    {
        v_shader_stream << "#define ALBEDO_MAP\n";
        f_shader_stream << "#define ALBEDO_MAP\n";
    }
    if(sShaderType.hasAlbedoMap())
    {
        v_shader_stream << "#define ALBEDO_MAP\n";
        f_shader_stream << "#define ALBEDO_MAP\n";
    }

    pShader->init("shaders/metagbuffer.frag", "shaders/metagbuffer.vert");

    return pShader;
}

void GeometryRenderProcess::initShaders()
{
    if(m_aShaders == nullptr)
    {
        delete[] m_aShaders;
    }
    m_aShaders = new Shader[s_uiNumShaderTypes];

    for(unsigned int i = 0; i < s_uiNumShaderTypes; ++i)
    {
        const GeometryRenderProcess::GeometryPassShaderType sShaderType(i);
        m_aShaders = generateShader(sShaderType);
    }
}
