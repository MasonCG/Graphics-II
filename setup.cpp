#include "Globals.h"
#include "Framework.h"
#include "shaders/importantconstants.h"
#include "Camera.h"
#include "Light.h"
#include <SDL.h>
#include "gltf.h"
#include "Text.h"
#include "BlitSquare.h"

void setup(Globals* globs)
{
    ShaderManager::initialize(globs->ctx);
    ImageManager::initialize(globs->ctx);

    globs->keepLooping = true;
    globs->framebuffer = new Framebuffer(globs->ctx);


    globs->nearestSampler = new Sampler(globs->ctx,
        VK_FILTER_NEAREST,
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        false
    );
    globs->linearSampler = new Sampler(globs->ctx,
        VK_FILTER_LINEAR,
        VK_SAMPLER_ADDRESS_MODE_REPEAT,
        false
    );
    globs->mipSampler = new Sampler(globs->ctx,
        VK_FILTER_LINEAR,
        VK_SAMPLER_ADDRESS_MODE_REPEAT,
        true
    );

    globs->camera = new Camera(
        vec3(0, 1, 2),    //eye
        vec3(0, 0, 0),    //coi
        vec3(0, 1, 0),    //up
        radians(35.0f), //field of view
        1.0f,           //aspect ratio
        0.1f,           //hither
        100.0f          //yon
    );

    globs->vertexManager = new VertexManager(
        globs->ctx,
        {
            VertexManager::Input{
                .name = "positions",
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .location = VS_INPUT_POSITION,
                .rate = VK_VERTEX_INPUT_RATE_VERTEX
            },
            VertexManager::Input{
                .name = "texcoords",
                .format = VK_FORMAT_R32G32_SFLOAT,
                .location = VS_INPUT_TEXCOORD,
                .rate = VK_VERTEX_INPUT_RATE_VERTEX
            },
            VertexManager::Input{
                .name = "normals",
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .location = VS_INPUT_NORMAL,
                .rate = VK_VERTEX_INPUT_RATE_VERTEX
            },

            VertexManager::Input{
                .name = "tangents",
                .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                .location = VS_INPUT_TANGENT,
                .rate = VK_VERTEX_INPUT_RATE_VERTEX
            },

            VertexManager::Input{
                .name = "texcoords2",
                .format = VK_FORMAT_R32G32_SFLOAT,
                .location = VS_INPUT_TEXCOORD2,
                .rate = VK_VERTEX_INPUT_RATE_VERTEX
            }

        }
    );

    globs->descriptorSetLayout = new DescriptorSetLayout(
        globs->ctx,
        {
            DescriptorSetEntry(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                UNIFORM_SLOT),
            DescriptorSetEntry(VK_DESCRIPTOR_TYPE_SAMPLER,
                                NEAREST_SAMPLER_SLOT),
            DescriptorSetEntry(VK_DESCRIPTOR_TYPE_SAMPLER,
                                LINEAR_SAMPLER_SLOT),
            DescriptorSetEntry(VK_DESCRIPTOR_TYPE_SAMPLER,
                                MIPMAP_SAMPLER_SLOT),
            DescriptorSetEntry(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                                BASE_TEXTURE_SLOT),
            DescriptorSetEntry(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                                EMISSIVE_TEXTURE_SLOT),
            DescriptorSetEntry(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                                NORMAL_TEXTURE_SLOT),
             DescriptorSetEntry(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                                METALLICROUGHNESS_TEXTURE_SLOT),
            DescriptorSetEntry(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                                ENVMAP_TEXTURE_SLOT),
             DescriptorSetEntry(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                                SUNFBO_TEXTURE_SLOT)
        }
    );


    globs->pushConstants = new PushConstants(
        "shaders/pushconstants.txt"
    );

    globs->pipelineLayout = new PipelineLayout(
        globs->ctx,
        globs->pushConstants->byteSize,
        globs->descriptorSetLayout,
        "default layout");

    globs->pipeline = new GraphicsPipeline(
        globs->ctx,
        "main pipe",
        globs->pipelineLayout,
        PipelineOption{ .shader = ShaderManager::load("shaders/main.vert") },
        PipelineOption{ .shader = ShaderManager::load("shaders/main.frag") },
        PipelineOption{ .vertexInputState = globs->vertexManager->inputState }
    );

    // skybox code
    globs->interiorEnviormentMap = ImageManager::loadCube(
        {
            "assets/kitchenMap/nx.jpg",
            "assets/kitchenMap/ny.jpg",
            "assets/kitchenMap/nz.jpg",
            "assets/kitchenMap/px.jpg",
            "assets/kitchenMap/py.jpg",
            "assets/kitchenMap/pz.jpg"
        }
    );

    globs->skyboxEnviormentMap = ImageManager::loadCube(
        {
            "assets/skymap/nx.jpg",
            "assets/skymap/ny.jpg",
            "assets/skymap/nz.jpg",
            "assets/skymap/px.jpg",
            "assets/skymap/py.jpg",
            "assets/skymap/pz.jpg"
        }
    );

    globs->skyboxCube = gltf::load("assets/cube.glb", globs->vertexManager)[0];

    
    globs->skyboxPipeline = new GraphicsPipeline(
        globs->ctx,
        "skybox pipe",
        globs->pipelineLayout,
        PipelineOption{ .shader = ShaderManager::load("shaders/sky.vert") },
        PipelineOption{ .shader = ShaderManager::load("shaders/sky.frag") },
        PipelineOption{ .vertexInputState = globs->vertexManager->inputState }
    );

    // end skybox code

   // shadows code




    globs->pipelineNonFloor = new GraphicsPipeline(
        globs->ctx,
        "nonfloor pipe",
        globs->pipelineLayout,
        PipelineOption{ .shader = ShaderManager::load("shaders/main.vert") },
        PipelineOption{ .shader = ShaderManager::load("shaders/main.frag") },
        PipelineOption{ .vertexInputState = globs->vertexManager->inputState }
    );

    globs->pipelineFloor = new GraphicsPipeline(
        globs->ctx,
        "floor pipe",
        globs->pipelineLayout,
        PipelineOption{ .shader = ShaderManager::load("shaders/main.vert") },
        PipelineOption{ .shader = ShaderManager::load("shaders/main.frag") },
        PipelineOption{ .vertexInputState = globs->vertexManager->inputState },
        PipelineOption{ .stencilTestEnable = 1 },
        PipelineOption{
            .stencilFrontAndBack = VkStencilOpState{
                .failOp = VK_STENCIL_OP_KEEP,
                .passOp = VK_STENCIL_OP_REPLACE,
                .depthFailOp = VK_STENCIL_OP_KEEP,
                .compareOp = VK_COMPARE_OP_ALWAYS,
                .compareMask = 0xff,
                .writeMask = 0xff,
                .reference = 1
            }
        }
    );

    globs->pipelineShadow = new GraphicsPipeline(
        globs->ctx,
        "shadow pipe",
        globs->pipelineLayout,
        PipelineOption{ .shader = ShaderManager::load("shaders/shadow.vert") },
        PipelineOption{ .shader = ShaderManager::load("shaders/shadow.frag") },
        PipelineOption{ .vertexInputState = globs->vertexManager->inputState },
        PipelineOption{ .depthTestEnable = 0 },
        PipelineOption{ .stencilTestEnable = 1 },
        PipelineOption{
            .stencilFrontAndBack = VkStencilOpState{
                .failOp = VK_STENCIL_OP_KEEP,
                .passOp = VK_STENCIL_OP_INCREMENT_AND_CLAMP,
                .depthFailOp = VK_STENCIL_OP_KEEP,
                .compareOp = VK_COMPARE_OP_EQUAL,
                .compareMask = 0xff,
                .writeMask = 0xff,
                .reference = 1
            }
        },
        PipelineOption{ .blendEnable = 1 },
        PipelineOption{ .srcColorBlendFactor = VK_BLEND_FACTOR_ZERO },
        PipelineOption{ .dstColorBlendFactor = VK_BLEND_FACTOR_ONE }
    );

    globs->pipelineFloorShadow = new GraphicsPipeline(
        globs->ctx,
        "floor shadow pipe",
        globs->pipelineLayout,
        PipelineOption{ .shader = ShaderManager::load("shaders/main.vert") },
        PipelineOption{ .shader = ShaderManager::load("shaders/main.frag") },
        PipelineOption{ .vertexInputState = globs->vertexManager->inputState },
        PipelineOption{ .depthTestEnable = 0 },
        PipelineOption{ .stencilTestEnable = 1 },
        PipelineOption{
            .stencilFrontAndBack = VkStencilOpState{
                .failOp = VK_STENCIL_OP_KEEP,
                .passOp = VK_STENCIL_OP_KEEP,
                .depthFailOp = VK_STENCIL_OP_KEEP,
                .compareOp = VK_COMPARE_OP_EQUAL,
                .compareMask = 0xff,
                .writeMask = 0xff,
                .reference = 2
            }
        },
        PipelineOption{ .blendEnable = 1 },
        PipelineOption{ .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA },
        PipelineOption{ .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA }
    );

    // end shadows lab

    // framebuffering and FBO lab & blur lab
    
    globs->offscreen = new BlurrableFramebuffer(
        globs->ctx,
        globs->width, globs->height, //width, height
        1,                          //num layers
        VK_FORMAT_R8G8B8A8_UNORM,   //format
        "fbo"                       //debugging name
    );

    globs->blitPipe = new GraphicsPipeline(
        globs->ctx,
        "blit pipe",
        globs->pipelineLayout,
        PipelineOption{ .shader = ShaderManager::load("shaders/blit.vert") },
        PipelineOption{ .shader = ShaderManager::load("shaders/blit.frag") },
        PipelineOption{ .vertexInputState = globs->vertexManager->inputState }
    );

    

    // end framebuffering FBO lab

    globs->descriptorSetFactory = new DescriptorSetFactory(
        globs->ctx,
        "main",
        0,      //binding point
        globs->pipelineLayout
    );
    globs->descriptorSet = globs->descriptorSetFactory->make();

    globs->uniforms = new Uniforms(
        globs->ctx,
        UNIFORM_SLOT,
        "shaders/uniforms.txt",
        "main uniforms"
    );

    globs->room = gltf::load("assets/room3.glb",globs->vertexManager);
    auto lights = gltf::getLights("assets/room3.glb");

    // framebuffer and FBO Lab
    globs->blitSquare = new BlitSquare(globs->vertexManager);

    // blur lab
    globs->offscreen = new BlurrableFramebuffer(
        globs->ctx,
        globs->width, globs->height, //width, height
        1,                          //num layers
        VK_FORMAT_R8G8B8A8_UNORM,   //format
        "fbo"                       //debugging name
    );


    lights[0].position;
    mat4 T = translation(-lights[0].position);
    mat4 Ti = translation(lights[0].position);
    vec4 N(0, 1, 0, 0);
    vec4 P(10, -0.3135, 20, 0);
    vec4 Pp = P * Ti;
    float D = -((N.x * Pp.x) + (N.y * Pp.y) + (N.z * Pp.z));
    mat4 M = mat4(
        -D, 0, 0, N.x,
        0, -D, 0, N.y,
        0, 0, -D, N.z,
        0, 0, 0, 0);

    globs->flattenMatrix = -(M * T * Ti);
    

    for( Light L : lights ){

        //xyz=position if positional; direction if directional. w=flag
        globs->lightPositionAndDirectionalFlag.push_back(
            vec4( L.position.x, L.position.y, L.position.z,
                  (L.positional ? 1.0f : 0.0f )
            )
        );

        //xyz=color, w=intensity
        globs->lightColorAndIntensity.push_back(
            vec4( L.color, L.intensity )
        );

        //x=inner, y=outer, zw=ignored
        globs->cosSpotAngles.push_back(
            vec4(L.cosInnerSpotAngle, L.cosOuterSpotAngle, 0,0 )
        );

        //xyz=direction, w=ignored
        globs->spotDirection.push_back(
            vec4(L.direction.xyz(), 0)
        );
    }

    globs->text = new Text(globs->ctx, globs->framebuffer,
        "assets/writing36.png",
        "assets/writing36.txt"
    );

    globs->vertexManager->pushToGPU();
    ImageManager::pushToGPU();
    globs->descriptorSet->setSlot( NEAREST_SAMPLER_SLOT, globs->nearestSampler->sampler );
    globs->descriptorSet->setSlot( LINEAR_SAMPLER_SLOT, globs->linearSampler->sampler );
    globs->descriptorSet->setSlot( MIPMAP_SAMPLER_SLOT, globs->mipSampler->sampler );

    SDL_SetRelativeMouseMode(SDL_TRUE);
    globs->mouseLook = true;
}
