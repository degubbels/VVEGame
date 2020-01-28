/**
* The Vienna Vulkan Engine
*
* (c) bei Helmut Hlavacs, University of Vienna
*
*/

#ifndef VESUBRENDERFWGLOW_H
#define VESUBRENDERFWGLOW_H

namespace ve {

	/**
	* \brief Subrenderer that manages entities that have a diffuse texture and a normal map
	*/
	class VESubrenderFW_Glow : public VESubrenderFW {
	public:
		///Constructor for class VESubrenderFW_DN
		VESubrenderFW_Glow() { };
		///Destructor for class VESubrenderFW_DN
		virtual ~VESubrenderFW_Glow() {};

		///\returns the class of the subrenderer
		virtual veSubrenderClass getClass() { return VE_SUBRENDERER_CLASS_POSTPROCESS; };
		///\returns the type of the subrenderer
		virtual veSubrenderType getType() { return VE_SUBRENDERER_TYPE_NONE; };

		//VkResult createPipeline(VkDevice device, VkGraphicsPipelineCreateInfo pipelineCreateInfo, VkPipeline* fullscreenPipeline);
		virtual void initSubrenderer();
		virtual void setDynamicPipelineState(VkCommandBuffer commandBuffer, uint32_t numPass);
		//void bindDescriptorSetImage(VkCommandBuffer commandBuffer, uint32_t imageIndex, VkFramebuffer frameBuffer);
		virtual void draw(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t numPass, VECamera* pCamera, VELight* pLight, std::vector<VkDescriptorSet> descriptorSetsShadow);
		void bindPipeline(VkCommandBuffer buffer);
		//virtual void addEntity(VEEntity* pEntity);
	};
}


#endif
