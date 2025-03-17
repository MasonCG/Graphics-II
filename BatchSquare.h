//BlitSquare.h

#pragma once
#include "vkhelpers.h"
#include "VertexManager.h"

class DescriptorSet;
class Image;

class BatchSquare {
public:
    VertexManager::Info drawinfo;
    BatchSquare(VertexManager* vertexManager);
    void draw(VulkanContext* ctx, VkCommandBuffer cmd, DescriptorSet* descriptorSet, Image* img);
    void drawInstanced(VkCommandBuffer cmd, unsigned numInstances);
    BatchSquare(const BatchSquare&) = delete;
    void operator=(const BatchSquare&) = delete;
}; 