#include "Framework.h"
#include "math2801.h"
#include "Globals.h"
#include "Uniforms.h"
#include "gltf.h"
#include "Text.h"
#include "shaders/importantconstants.h"
#include "BlitSquare.h"
#include "Framebuffer.h"




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

    globs->offscreen->beginRenderPassClearContents(cmd, 0, 0, 0, 0); //0.2f, 0.4f, 0.6f, 1.0f );

    globs->descriptorSet->bind(cmd);
    globs->vertexManager->bindBuffers(cmd);

    globs->camera->setUniforms(globs->uniforms);

    globs->uniforms->set( "attenuation", vec3(75,1,100) );
    globs->uniforms->set( "lightPositionAndDirectionalFlag", globs->lightPositionAndDirectionalFlag );
    globs->uniforms->set( "lightColorAndIntensity", globs->lightColorAndIntensity );
    globs->uniforms->set( "cosSpotAngles", globs->cosSpotAngles );
    globs->uniforms->set( "spotDirection", globs->spotDirection );
    globs->uniforms->set("flattenMatrix", globs->flattenMatrix);
    globs->uniforms->set("reflectionMatrix", globs->reflectionMatrix);
    globs->uniforms->set("reflectionPlane", globs->reflectionPlane);


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

