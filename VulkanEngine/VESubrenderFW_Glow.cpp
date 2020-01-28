/**
* degu
*
*/


#include "VEInclude.h"


namespace ve {

	/**
	* \brief Initialize the subrenderer
	*
	* Create descriptor set layout, pipeline layout and the PSO
	*
	*/
	void VESubrenderFW_Glow::initSubrenderer() {


		//std::vector<VkPipeline>			m_pipelines;
		//m_pipelines.resize(1);
		//VkPipeline* fullscreenPipeline;// = &m_pipelines[0];

		//VkPipelineVertexInputStateCreateInfo emptyInputState;
		//emptyInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		//emptyInputState.vertexAttributeDescriptionCount = 0;
		//emptyInputState.pVertexAttributeDescriptions = nullptr;
		//emptyInputState.vertexBindingDescriptionCount = 0;
		//emptyInputState.pVertexBindingDescriptions = nullptr;

		//// Define shaders

		//VkDevice device = getRendererForwardPointer()->getDevice();
		//std::vector<std::string> shaderFileNames = {
		//	"media/shader/Forward/Glow/vert.spv",
		//	"media/shader/Forward/Glow/frag.spv"
		//};
		//std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

		//auto vertShaderCode = vh::vhFileRead(shaderFileNames[0]);

		//// Create shader module
		//VkShaderModuleCreateInfo createInfo = {};
		//createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		//createInfo.codeSize = vertShaderCode.size();
		//createInfo.pCode = reinterpret_cast<const uint32_t*>(vertShaderCode.data());

		//VkShaderModule vertShaderModule;
		//if (vkCreateShaderModule(device, &createInfo, nullptr, &vertShaderModule) != VK_SUCCESS) {
		//	assert(false);
		//	exit(1);
		//}

		//VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
		//vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		//vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		//vertShaderStageInfo.module = vertShaderModule;
		//vertShaderStageInfo.pName = "main";

		//shaderStages.push_back(vertShaderStageInfo);

		//VkShaderModule fragShaderModule = VK_NULL_HANDLE;
		//if (shaderFileNames.size() > 1 && shaderFileNames[1].size() > 0) {
		//	auto fragShaderCode = vh::vhFileRead(shaderFileNames[1]);

		//	// Create shader module
		//	VkShaderModuleCreateInfo createInfo = {};
		//	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		//	createInfo.codeSize = fragShaderCode.size();
		//	createInfo.pCode = reinterpret_cast<const uint32_t*>(fragShaderCode.data());

		//	if (vkCreateShaderModule(device, &createInfo, nullptr, &fragShaderModule) != VK_SUCCESS) {
		//		assert(false);
		//		exit(1);
		//	}

		//	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
		//	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		//	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		//	fragShaderStageInfo.module = fragShaderModule;
		//	fragShaderStageInfo.pName = "main";

		//	shaderStages.push_back(fragShaderStageInfo);
		//}


		//VkGraphicsPipelineCreateInfo pipelineCreateInfo;
		//pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		//pipelineCreateInfo.pVertexInputState = &emptyInputState;
		//pipelineCreateInfo.pStages = shaderStages.data();

		//m_pipelines.resize(1);
		//vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_pipelines[0]);

		/*VkResult ret = VHCHECKRESULT(vkCreateGraphicsPipelines(device,
			VK_NULL_HANDLE, 
			1, 
			&pipelineCreateInfo, 
			nullptr, 
			fullscreenPipeline) );*/

		/*VkResult ret = createPipeline(device, pipelineCreateInfo, fullscreenPipeline);
		printf("ret: %d\n", ret);*/

		//VkCommandBuffer buff;// INITIALIZE BUFFER
		//vkCmdBindPipeline(buff, VK_PIPELINE_BIND_POINT_GRAPHICS, fullscreenPipeline);
		//vkCmdDraw(buff, 3, 1, 0, 0);

		VESubrenderFW::initSubrenderer();
		

		vh::vhRenderCreateDescriptorSetLayout(getRendererForwardPointer()->getDevice(),		//binding 0...array, binding 1...array
			{ m_resourceArrayLength,						m_resourceArrayLength },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER },
			{ VK_SHADER_STAGE_FRAGMENT_BIT,					VK_SHADER_STAGE_FRAGMENT_BIT },
			&m_descriptorSetLayoutResources);

		VkDescriptorSetLayout perObjectLayout = getRendererForwardPointer()->getDescriptorSetLayoutPerObject();

		vh::vhPipeCreateGraphicsPipelineLayout(getRendererForwardPointer()->getDevice(),
			{ perObjectLayout, perObjectLayout,  getRendererForwardPointer()->getDescriptorSetLayoutShadow(),
				perObjectLayout, m_descriptorSetLayoutResources },
			{}, &m_pipelineLayout);

		m_pipelines.resize(1);
		vh::vhPipeCreateGraphicsPostProcessPipeline(getRendererForwardPointer()->getDevice(),
			{ "media/shader/Forward/Glow/vert.spv", "media/shader/Forward/Glow/frag.spv" },
			getRendererForwardPointer()->getSwapChainExtent(),
			m_pipelineLayout, getRendererForwardPointer()->getRenderPass(),
			{ VK_DYNAMIC_STATE_BLEND_CONSTANTS },
			&m_pipelines[0]);

		if (m_maps.empty()) m_maps.resize(2);
		printf("Glow rendere initialized\n");
	}

	/**
	* \brief Set the danymic pipeline stat, i.e. the blend constants to be used
	*
	* \param[in] commandBuffer The currently used command buffer
	* \param[in] numPass The current pass number - in the forst pass, write over pixel colors, after this add pixel colors
	*
	*/
	void VESubrenderFW_Glow::setDynamicPipelineState(VkCommandBuffer commandBuffer, uint32_t numPass) {
		//if (numPass == 0) {
		//	float blendConstants[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		//	vkCmdSetBlendConstants(commandBuffer, blendConstants);
		//	return;
		//}

		float blendConstants[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		vkCmdSetBlendConstants(commandBuffer, blendConstants);
	}

	//void VESubrenderFW_Glow::bindDescriptorSetImage(VkCommandBuffer commandBuffer, uint32_t imageIndex, VkFramebuffer frameBuffer) {

	//	std::vector<VkDescriptorSet> sets = {};
	//	//sets.push_back(m_descriptorSetsResources[entity->getResourceIdx() / m_resourceArrayLength]);
	//	

	//	uint32_t offset = entity->m_memoryHandle.entryIndex * sizeof(VEEntity::veUBOPerEntity_t);
	//	vkCmdBindDescriptorSets(m_commandBuffers[m_imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout,
	//		4, 1, sets.data(), 1, &offset);

	//}

	/**
	* \brief Add an entity to the subrenderer
	*
	* Create a UBO for this entity, a descriptor set per swapchain image, and update the descriptor sets
	*
	*/
	//void VESubrenderFW_Glow::addEntity(VEEntity* pEntity) {

	//	std::vector<VkDescriptorImageInfo> maps = {
	//		pEntity->m_pMaterial->mapDiffuse->m_imageInfo,
	//		pEntity->m_pMaterial->mapNormal->m_imageInfo
	//	};

	//	addMaps(pEntity, maps);

	//	VESubrender::addEntity(pEntity);

	//}

	void VESubrenderFW_Glow::draw(VkCommandBuffer commandBuffer, uint32_t imageIndex,
		uint32_t numPass,
		VECamera* pCamera, VELight* pLight,
		std::vector<VkDescriptorSet> descriptorSetsShadow) {

		printf("Glow::Draw, this=%X\n", this->getClass());

		//vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelines[0]);

		__super::draw(commandBuffer, imageIndex, 
			numPass, 
			pCamera, pLight, 
			descriptorSetsShadow);
	}

	void VESubrenderFW_Glow::bindPipeline(VkCommandBuffer buffer) {
		vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelines[0]);
	}
}


