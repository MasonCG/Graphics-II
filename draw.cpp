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

    // Flare Lab
    globs->sunfbo->beginRenderPassClearContents(cmd, 0.0f, 0.0f, 0.0f, 1.0f);

    globs->descriptorSet->bind(cmd);
    globs->vertexManager->bindBuffers(cmd);
    globs->uniforms->set("attenuation", vec3(500, 0, 1));
    globs->uniforms->set("lightPositionAndDirectionalFlag", globs->lightPositionAndDirectionalFlag);
    globs->uniforms->set("lightColorAndIntensity", globs->lightColorAndIntensity);
    globs->uniforms->set("cosSpotAngles", globs->cosSpotAngles);
    globs->uniforms->set("spotDirection", globs->spotDirection);
    
    globs->uniforms->bind(cmd, globs->descriptorSet);


    globs->blackSunPipe->use(cmd);
    for (auto& m : globs->room) {
        m->draw(globs->ctx, cmd, globs->descriptorSet, globs->pushConstants);
    }
    globs->whiteSunPipe->use(cmd);
    globs->blitSquare->draw(globs->ctx, cmd, globs->descriptorSet, nullptr);




    globs->sunfbo->endRenderPass(cmd);


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

    globs->uniforms->set("windowP0", globs->windowP0);
    globs->uniforms->set("windowUvec", globs->windowUvec);
    globs->uniforms->set("windowVvec", globs->windowVvec);
    globs->uniforms->set("windowp1p0", globs->windowp1p0);
    globs->uniforms->set("windowp3p0", globs->windowp3p0);
    globs->uniforms->set("windowPlane", globs->windowPlane);
    globs->uniforms->set("windowLightDirection", globs->windowLightDirection);

    globs->descriptorSet->setSlot(PROJECTED_TEXTURE_SLOT, globs->windowImage->view());

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
    globs->offscreen->blur(1, 0, 1.0f, cmd, globs->vertexManager);


    globs->vertexManager->bindBuffers(cmd);

    globs->framebuffer->beginRenderPassClearContents(cmd, 0.2f, 0.4f, 0.6f, 1.0f);

    globs->blitPipe->use(cmd);

    globs->blitSquare->draw(
        globs->ctx,
        cmd,
        globs->descriptorSet,
        globs->offscreen->currentImage()
    );


    /*
    globs->pushConstants->set(cmd, "doingShadow", 0);
    for (gltf::Mesh* m : globs->room) {
        m->draw(globs->ctx, cmd, globs->descriptorSet, globs->pushConstants);
    }

    globs->pushConstants->set(cmd, "doingShadow", 1);
    for (gltf::Mesh* m : globs->room) {
        if (m->name != "floor" && m->name != "room") {
            m->draw(globs->ctx, cmd, globs->descriptorSet, globs->pushConstants);
        }
    }
    
    globs->pipelineNonFloor->use(cmd);
    globs->pushConstants->set(cmd, "doingShadow", 0);
    for (auto& m : globs->room) {
        if (m->name != "floor") {
            m->draw(globs->ctx, cmd, globs->descriptorSet, globs->pushConstants);
        }
    }

    globs->pipelineFloor->use(cmd);
    for (auto& m : globs->room) {
        if (m->name == "floor") {
            m->draw(globs->ctx, cmd, globs->descriptorSet, globs->pushConstants);
        }
    }

    globs->pipelineShadow->use(cmd);
    globs->pushConstants->set(cmd, "doingShadow", 1);

    for (auto& m : globs->room) {
        if (m->name != "floor" && m->name != "room") {
            m->draw(globs->ctx, cmd, globs->descriptorSet, globs->pushConstants);
        }
    }

    globs->pipelineFloorShadow->use(cmd);
    globs->pushConstants->set(cmd, "doingShadow", 1);
    for (auto& m : globs->room) {
        if (m->name == "floor") {
            m->draw(globs->ctx, cmd, globs->descriptorSet, globs->pushConstants);
        }
    }

    */



   

   

    // START: Flare Lab
    /*

    vec3 lightPos = globs->lightPositionAndDirectionalFlag[0].xyz();
    float lightIsPositional = globs->lightPositionAndDirectionalFlag[0].w;
    if (!lightIsPositional) {
        //directional, so lightPositionAndDirectionalFlag
        //holds direction to the light
        lightPos = globs->camera->eye + 100 * lightPos;
    }
    vec4 S = vec4(lightPos, 1.0) * globs->camera->viewMatrix * globs->camera->projMatrix;
    S = 1.0 / S.w * S;        //screen space loc of light source, -1...1

    const float fadeStart = 0.8f;
    const float fadeEnd = 1.2f;
    float tmp = std::max(std::abs(S.x), std::abs(S.y));
    float alpha;
    if (tmp < fadeStart) {
        //no fade
        alpha = 1.0f;
    }
    else if (tmp > fadeEnd) {
        //100% transparent
        alpha = 0.0f;
    }
    else {
        alpha = 1.0f - (tmp - fadeStart) / (fadeEnd - fadeStart);
    }
    if (dot(globs->camera->look, lightPos - globs->camera->eye) < 0.0)
        alpha = 0.0f;


    vec3 sunup(0, 1, 0);
    vec3 sunlook = lightPos - globs->camera->eye;
    if (dot(sunlook, sunup) < 0.1)
        sunup = vec3(0, 0, 1);
    globs->suncamera->lookAt(globs->camera->eye, lightPos, sunup);
    globs->suncamera->setUniforms(globs->uniforms);
    globs->uniforms->set("viewMatrix", mat4::identity());
    globs->uniforms->set("projMatrix", mat4::identity());
    globs->uniforms->set("viewProjMatrix", mat4::identity());
    globs->uniforms->bind(cmd, globs->descriptorSet);

    globs->pushConstants->set(cmd, "baseColorFactor", vec4(1, 1, 1, alpha));

    globs->pushConstants->set(cmd, "worldMatrix", scaling(0.25, 0.25, 0.25) * translation(S.xyz()));
   

    globs->flarePipeline->use(cmd);

    globs->descriptorSet->setSlot(SUNFBO_TEXTURE_SLOT, globs->sunfbo->currentImage()->view());

    globs->blitSquare->draw(globs->ctx, cmd,
        globs->descriptorSet, globs->glowTexture);

    for (auto& tpl : flareSpec) {
        int textureIndex = std::get<0>(tpl);
        float t = std::get<1>(tpl);
        float scale = std::get<2>(tpl);
        vec4 q = S - S * t;
        globs->pushConstants->set(cmd, "worldMatrix", scaling(scale, scale, scale) * translation(q.xyz()));
        globs->blitSquare->draw(globs->ctx, cmd,
            globs->descriptorSet, globs->flares[textureIndex]);
    }



    
    */
    // END: Flare Lab

    globs->framebuffer->endRenderPass(cmd);
    vkEndCommandBuffer(cmd);

    utils::endFrame(globs->ctx,cmd);
}

