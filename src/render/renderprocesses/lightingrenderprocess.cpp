#include "lightingrenderprocess.h"

#include "framebuffer.h"
#include "renderingquad.h"
#include "shader.h"
#include "texture.h"

LightingRenderProcess::LightingRenderProcess() :
    m_pOutBuffer(nullptr),
    m_pLightingShader(nullptr),
    m_pTexture2dDepthShader(nullptr),
    m_pCubemapTextureDepthShader(nullptr)
{

}

LightingRenderProcess::~LightingRenderProcess()
{
    if(m_pOutBuffer != nullptr)
    {
        delete m_pOutBuffer;
    }

    if(m_pLightingShader != nullptr)
    {
        delete m_pLightingShader;
    }

    if(m_pTexture2dDepthShader != nullptr)
    {
        delete m_pTexture2dDepthShader;
    }

    if(m_pCubemapTextureDepthShader != nullptr)
    {
        delete m_pCubemapTextureDepthShader;
    }
}

void LightingRenderProcess::init(const GLsizei &iWidth, const GLsizei &iHeight)
{
    if(m_bInitialized == false)
    {
        ARenderProcess::init(iWidth, iHeight);

        m_pTexture2dDepthShader->init("shaders/texture_2D_depth.vert", "shaders/texture_2D_depth.frag");
        m_pCubemapTextureDepthShader->init("shaders/cubemap_depth.vert", "shaders/cubemap_depth.frag", "shaders/cubemap_depth.geom");

        m_pLightingShader->init("shaders/metalighting.vert", "shaders/metalighting.frag");

        m_pLightingShader->setFragmentDefineEnabled("dirlight", false);
        m_pLightingShader->setFragmentDefineEnabled("pointlight", false);
        m_pLightingShader->setFragmentDefineEnabled("spotlight", false);

        m_pOutBuffer->init(iWidth, iHeight);
        std::vector<TextureAttachment> aBlurTexturesAttachments;
        aBlurTexturesAttachments.push_back({new Texture(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_BORDER), GL_RGB16F});
        aBlurTexturesAttachments.push_back({new Texture(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_BORDER), GL_RGB16F});
        aBlurTexturesAttachments.push_back({new Texture(GL_R16F, GL_RED, GL_FLOAT, GL_CLAMP_TO_BORDER), GL_R16F});
        m_pOutBuffer->attachTextures(aBlurTexturesAttachments);


        //  Prepare inputs
        m_mapInputs["position_depth"] = {GL_RGBA16F, "", nullptr};
        m_mapInputs["normal"] = {GL_RGB16F, "", nullptr};
        m_mapInputs["albedo_spec"] = {GL_RGBA16F, "", nullptr};
        m_mapInputs["roughness_ior_metalness"] = {GL_RGB16F, "", nullptr};
        m_mapInputs["ambient_occlusion"] = {GL_R16F, "", nullptr};

        //  Prepare outputs
        m_mapOutTextures["fragment_color"] = m_pOutBuffer->getTexture(0);
        m_mapOutTextures["bright_color"] = m_pOutBuffer->getTexture(1);
        m_mapOutTextures["brightness"] = m_pOutBuffer->getTexture(2);
    }
}

void LightingRenderProcess::resize(const GLsizei &iWidth, const GLsizei &iHeight)
{
    ARenderProcess::resize(iWidth, iHeight);

    if(m_bInitialized)
    {
        m_pOutBuffer->resize(iWidth, iHeight);
    }
}

void LightingRenderProcess::process(const RenderingQuad *pRenderingQuad, const GLfloat &fRenderTime)
{
    processShadows(scene, fRenderTime, keys);
    glDisable(GL_DEPTH_TEST);
    m_buffer.bind();
    glViewport(0, 0, m_buffer.width(), m_buffer.height());
    m_lighting_shader.use();
    m_lighting_shader.resetActiveTexture();

    glActiveTexture(m_lighting_shader.getActiveTexture());
    m_lighting_shader.setNextTexture();
    bindPreviousTexture(0);
    glActiveTexture(m_lighting_shader.getActiveTexture());
    m_lighting_shader.setNextTexture();
    bindPreviousTexture(1);
    glActiveTexture(m_lighting_shader.getActiveTexture());
    m_lighting_shader.setNextTexture();
    bindPreviousTexture(2);
    glActiveTexture(m_lighting_shader.getActiveTexture());
    m_lighting_shader.setNextTexture();
    bindPreviousTexture(3);
    glActiveTexture(m_lighting_shader.getActiveTexture());
    m_lighting_shader.setNextTexture();
    bindPreviousTexture(4);

    scene.sendLightDatas(m_lighting_shader);
    scene.sendCameraToShader(m_lighting_shader, m_buffer.width(), m_buffer.height());

    m_lighting_shader.release();
}

void LightingRenderProcess::reloadShaders()
{

}

void LightingRenderProcess::allocate()
{
    if(m_pOutBuffer == nullptr)
    {
        m_pOutBuffer = new Framebuffer();
    }

    if(m_pLightingShader == nullptr)
    {
        m_pLightingShader = new Shader();
    }

    if(m_pTexture2dDepthShader == nullptr)
    {
        m_pTexture2dDepthShader = new Shader();
    }

    if(m_pCubemapTextureDepthShader == nullptr)
    {
        m_pCubemapTextureDepthShader = new Shader();
    }
}

void LightingRenderProcess::processShadows(const GLfloat &fRenderTime)
{
    glEnable(GL_DEPTH_TEST);

    m_pTexture2dDepthShader->use();

    //  DirLights
    for(size_t i = 0; i < scene.numberOfDirLights(); ++i)
    {
        DirLight *current_dirlight = scene.getDirLight(i);
        glViewport(0, 0, current_dirlight->getDepthMapWidth(), current_dirlight->getDepthMapHeight());
        current_dirlight->bindDepthMapBuffer();
        glClear(GL_DEPTH_BUFFER_BIT);

        m_2D_texture_depth_shader.resetActiveTexture();
        current_dirlight->sendShadowDatas(m_2D_texture_depth_shader);
        scene.drawNoLight(m_2D_texture_depth_shader, keys, fRenderTime, current_dirlight->getDepthMapWidth(), current_dirlight->getDepthMapHeight());

        current_dirlight->releaseDepthMapBuffer();
    }

    //  SpotLights
    for(size_t i = 0; i < scene.numberOfSpotLights(); ++i)
    {
        SpotLight *current_spotlight = scene.getSpotLight(i);
        glViewport(0, 0, current_spotlight->getDepthMapWidth(), current_spotlight->getDepthMapHeight());
        current_spotlight->bindDepthMapBuffer();
        glClear(GL_DEPTH_BUFFER_BIT);

        m_2D_texture_depth_shader.resetActiveTexture();
        current_spotlight->sendShadowDatas(m_2D_texture_depth_shader);
        scene.drawNoLight(m_2D_texture_depth_shader, keys, fRenderTime, current_spotlight->getDepthMapWidth(), current_spotlight->getDepthMapHeight());

        current_spotlight->releaseDepthMapBuffer();
    }

    m_cubemap_texture_depth_shader.use();

    //  PointLights
    for(size_t i = 0; i < scene.numberOfPointLights(); ++i)
    {
        PointLight *current_pointlight = scene.getPointLight(i);
        glViewport(0, 0, current_pointlight->getDepthMapWidth(), current_pointlight->getDepthMapHeight());
        current_pointlight->bindDepthMapBuffer();
        glClear(GL_DEPTH_BUFFER_BIT);

        m_cubemap_texture_depth_shader.resetActiveTexture();
        current_pointlight->sendShadowDatas(m_cubemap_texture_depth_shader);
        scene.drawNoLight(m_cubemap_texture_depth_shader, keys, fRenderTime, current_pointlight->getDepthMapWidth(), current_pointlight->getDepthMapHeight());

        current_pointlight->releaseDepthMapBuffer();
    }

    m_cubemap_texture_depth_shader.release();
}

void LightingRenderProcess::updateShader(const size_t &uiNbDirlights, const size_t &uiNbPointlights, const GLuint &uiNbSpotlights)
{
    if(uiNbDirlights > 0)
    {
        const std::string strDirlight("DIRLIGHT " + std::to_string(uiNbDirlights));
        m_pLightingShader->setFragmentDefineText("dirlight", strDirlight);
    }
    else
    {
        m_pLightingShader->setFragmentDefineEnabled("dirlight", false);
    }

    if(uiNbPointlights > 0)
    {
        const std::string strPointlight("POINTLIGHT " + std::to_string(uiNbPointlights));
        m_pLightingShader->setFragmentDefineText("pointlight", strPointlight);
    }
    else
    {
        m_pLightingShader->setFragmentDefineEnabled("pointlight", false);
    }

    if(uiNbSpotlights > 0)
    {
        const std::string strSpotlight("SPOTLIGHT " + std::to_string(uiNbSpotlights));
        m_pLightingShader->setFragmentDefineText("spotlight", strSpotlight);
    }
    else
    {
        m_pLightingShader->setFragmentDefineEnabled("spottlight", false);
    }

    m_pLightingShader->reload();
}
