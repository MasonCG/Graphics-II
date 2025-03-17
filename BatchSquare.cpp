//BlitSquare.cpp

#include "BatchSquare.h"
#include "Framework.h"
#include <vector>
#include "shaders/importantconstants.h"

using namespace math2801;



BatchSquare::BatchSquare(VertexManager* vertexManager) {

    std::vector<vec3> pos;
    std::vector<vec2> tex;
    std::vector<vec3> nor;
    std::vector<vec4> tan;
    std::vector<vec2> tex2;

    std::vector<uint32_t> ind;


    for (int i = 0; i < BILLBOARD_BATCH_SIZE; i++) {

        pos.push_back(vec3{ i, -1, 0 });
        pos.push_back(vec3{ i, -1, 0 });
        pos.push_back(vec3{ i, 1, 0 });
        pos.push_back(vec3{ i, 1, 0 });
        tex.push_back(vec2{ 0, 0 });
        tex.push_back(vec2{ 1, 0 });
        tex.push_back(vec2{ 1, 1 });
        tex.push_back(vec2{ 0, 1});
        nor.push_back(vec3{ 0, 0, 1 });
        nor.push_back(vec3{ 1, -1, 1 });
        nor.push_back(vec3{ 0, 0, 1 });
        nor.push_back(vec3{ 0, 0, 1 });
        tan.push_back(vec4{ 0, 0, 0, 0 });
        tan.push_back(vec4{ 0,0,0,0});
        tan.push_back(vec4{ 0,0,0,0 });
        tan.push_back(vec4{ 0,0,0,0 });
        tex2.push_back(vec2{ 0, 0 });
        tex2.push_back(vec2{ 1, 0 });
        tex2.push_back(vec2{ 1, 1 });
        tex2.push_back(vec2{ 0, 1 });



        ind.push_back(0 + i * 4);
        ind.push_back(1 + i * 4);
        ind.push_back(2 + i * 4);
        ind.push_back(0 + i * 4);
        ind.push_back(2 + i * 4);
        ind.push_back(3 + i * 4);

    }



    this->drawinfo = vertexManager->addIndexedData(
        "positions",pos,

        "texcoords",tex,

        "normals",nor,

        "tangents",tan,

        "texcoords2",tex2,

        "indices", ind
    );
}

void BatchSquare::draw(VulkanContext* ctx, VkCommandBuffer cmd,
    DescriptorSet* descriptorSet, Image* img)
{
    ctx->beginCmdRegion(cmd, "BatchSquare");

    if (img != nullptr) {
        descriptorSet->setSlot(BASE_TEXTURE_SLOT, img->view());
        //these three are dummy
        descriptorSet->setSlot(EMISSIVE_TEXTURE_SLOT, img->view());
        descriptorSet->setSlot(NORMAL_TEXTURE_SLOT, img->view());
        descriptorSet->setSlot(METALLICROUGHNESS_TEXTURE_SLOT, img->view());

        descriptorSet->bind(cmd);
    }

    vkCmdDrawIndexed(
        cmd,
        6,              //index count
        1,              //instance count
        this->drawinfo.indexOffset,
        this->drawinfo.vertexOffset,
        0               //first instance
    );

    ctx->endCmdRegion(cmd);
}

void BatchSquare::drawInstanced(VkCommandBuffer cmd,
    unsigned numInstances)
{
    vkCmdDrawIndexed(
        cmd,
        6* BILLBOARD_BATCH_SIZE,              //index count
        numInstances,              //instance count
        this->drawinfo.indexOffset,
        this->drawinfo.vertexOffset,
        0               //first instance
    );
}