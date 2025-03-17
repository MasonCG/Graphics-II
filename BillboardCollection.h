#pragma once


//BillboardCollection.h
#pragma once

#include "vkhelpers.h"
#include "math2801.h"

class BatchSquare;
class Buffer;
class Image;
class DescriptorSet;
class VertexManager;


class BillboardCollection {
public:

    BillboardCollection(VulkanContext* ctx_,
        VertexManager* vertexManager,
        const std::vector<math2801::vec4>& positions,
        Image* img
    );

    void draw(VkCommandBuffer cmd,
        DescriptorSet* descriptorSet
    );

    //...data items...

    unsigned numBillboards;
    Image* img;
    BatchSquare* batchSquare;
    VkBufferView positionView;
    Buffer* positions;
    VulkanContext* ctx;





};