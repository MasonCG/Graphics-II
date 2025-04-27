#include "Framework.h"
#include "math2801.h"
#include "Globals.h"
#include "Uniforms.h"
#include "gltf.h"
#include "Text.h"
#include "shaders/importantconstants.h"
#include "BlitSquare.h"
#include "Framebuffer.h"


// flare lab
std::vector<std::tuple<int, float, float>> flareSpec{
    { 0, -0.6, 0.1},
    { 0, 0.25, 0.05 },
    { 0, 1.1, 0.15 },
    { 0, 1.7, 0.3 },
    { 1, -0.1, 0.14 },
    { 1, 0.3, 0.2 },
    { 1, 0.5, 0.1 },
    { 2, -0.4, 0.06 },
    { 2, 0.4, 0.04 },
    { 2, 0.7, 0.08 },
    { 2, 1.2, 0.16 }
};

void draw(Globals* globs)
{
    utils::beginFrame(globs->ctx);

    auto cmd = CommandBuffer::allocate();


    VkCommandBufferBeginInfo beginInfo {
        .sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext=nullptr,
        .flags=0,
        .pInheritanceInfo=nullptr
    };
    vkBeginCommandBuffer(cmd,&beginInfo);

    globs->descriptorSet->setSlot(
        ENVMAP_TEXTURE_SLOT,
        globs->interiorEnviormentMap->view()
    );


    globs->text->update(cmd);

    globs->offscreen->beginRenderPassClearContents(cmd, 0, 1, 0, 1); //0.2f, 0.4f, 0.6f, 1.0f );

    globs->descriptorSet->bind(cmd);
    globs->vertexManager->bindBuffers(cmd);

    globs->camera->setUniforms(globs->uniforms);

    globs->uniforms->set( "attenuation", vec3(100,1,100) );
    globs->uniforms->set( "lightPositionAndDirectionalFlag", globs->lightPositionAndDirectionalFlag );
    globs->uniforms->set( "lightColorAndIntensity", globs->lightColorAndIntensity );
    globs->uniforms->set( "cosSpotAngles", globs->cosSpotAngles );
    globs->uniforms->set( "spotDirection", globs->spotDirection );
    globs->uniforms->set("flattenMatrix", globs->flattenMatrix);

    globs->uniforms->bind(cmd,globs->descriptorSet);

    globs->pipeline->use(cmd);

  

    
    for (auto& m : globs->room) {
        m->draw(globs->ctx, cmd, globs->descriptorSet, globs->pushConstants);
    }

    // drawing skybox last
    globs->skyboxPipeline->use(cmd);
    globs->descriptorSet->setSlot(
        ENVMAP_TEXTURE_SLOT,
        globs->skyboxEnviormentMap->view()
    );
    globs->skyboxCube->draw(
        globs->ctx,
        cmd,
        globs->descriptorSet,
        globs->pushConstants
    );

    globs->text->draw(cmd);


    globs->offscreen->endRenderPass(cmd);

    //blur lab
    globs->offscreen->blur(3, 0, 1.0f, cmd, globs->vertexManager);


    globs->vertexManager->bindBuffers(cmd);

    globs->framebuffer->beginRenderPassClearContents(cmd, 0.2f, 0.4f, 0.6f, 1.0f);

    globs->blitPipe->use(cmd);

    globs->blitSquare->draw(
        globs->ctx,
        cmd,
        globs->descriptorSet,
        globs->offscreen->currentImage()
    );


    globs->framebuffer->endRenderPass(cmd);
    vkEndCommandBuffer(cmd);

    utils::endFrame(globs->ctx,cmd);
}

