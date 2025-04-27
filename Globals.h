#pragma once

#include <set>
#include <vector>

#include "math2801.h"
using namespace math2801;

class Text;
class Camera;
class VulkanContext;
class DescriptorSet;
class Sampler;
class VertexManager;
class Uniforms;
class Pipeline;
class PushConstants;
class Framebuffer;
class DescriptorSetLayout;
class DescriptorSet;
class DescriptorSetFactory;
class PipelineLayout;
class GraphicsPipeline;
class Image;
class BlitSquare;
class BlurrableFramebuffer;


namespace gltf{
  class Mesh;
};

class Globals{
  public:
    int width;
    int height;
    std::set<int> pressedKeys;
    bool keepLooping=true;
    VulkanContext* ctx;
    bool mouseLook = true;
    Camera* camera;
    Text* text;
    Framebuffer* framebuffer;
    DescriptorSet* descriptorSet;
    VertexManager* vertexManager;
    Uniforms* uniforms;
    Pipeline* pipeline;
    PushConstants* pushConstants;
    std::vector<gltf::Mesh*> room;
    std::vector<vec4> lightPositionAndDirectionalFlag;
    std::vector<vec4> lightColorAndIntensity;
    std::vector<vec4> cosSpotAngles;
    std::vector<vec4> spotDirection;
    Sampler* nearestSampler;
    Sampler* linearSampler;
    Sampler* mipSampler;
    DescriptorSetLayout* descriptorSetLayout;
    PipelineLayout* pipelineLayout;
    DescriptorSetFactory* descriptorSetFactory;
    
    //for skybox
    Pipeline* skyboxPipeline;
    Image* interiorEnviormentMap;
    Image* skyboxEnviormentMap;
    gltf::Mesh* skyboxCube;

    // for shading
    mat4 flattenMatrix;
    GraphicsPipeline* pipelineNonFloor;
    GraphicsPipeline* pipelineFloor;
    GraphicsPipeline* pipelineShadow;
    GraphicsPipeline* pipelineFloorShadow;

    // framebuffer FBO Lab
    BlurrableFramebuffer* offscreen;
    GraphicsPipeline* blitPipe;
    BlitSquare* blitSquare;

    //depth of field lab
    float focalDistance;


};