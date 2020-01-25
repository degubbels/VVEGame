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
		VESubrenderFW::initSubrenderer();

		std::vector<VkPipeline>			m_pipelines;
		m_pipelines.resize(1);
		VkPipeline *fullscreenPipeline = &m_pipelines[0];

		VkPipelineVertexInputStateCreateInfo emptyInputState;
		emptyInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		emptyInputState.vertexAttributeDescriptionCount = 0;
		emptyInputState.pVertexAttributeDescriptions = nullptr;
		emptyInputState.vertexBindingDescriptionCount = 0;
		emptyInputState.pVertexBindingDescriptions = nullptr;

		// Define shaders

		VkDevice device = getRendererForwardPointer()->getDevice();
		std::vector<std::string> shaderFileNames = {
			"media/shader/Forward/Glow/vert.spv",
			"media/shader/Forward/Glow/frag.spv"
		};
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

		auto vertShaderCode = vh::vhFileRead(shaderFileNames[0]);

		// Create shader module
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = vertShaderCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(vertShaderCode.data());

		VkShaderModule vertShaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &vertShaderModule) != VK_SUCCESS) {
			assert(false);
			exit(1);
		}

		VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		shaderStages.push_back(vertShaderStageInfo);

		VkShaderModule fragShaderModule = VK_NULL_HANDLE;
		if (shaderFileNames.size() > 1 && shaderFileNames[1].size() > 0) {
			auto fragShaderCode = vh::vhFileRead(shaderFileNames[1]);

			// Create shader module
			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = fragShaderCode.size();
			createInfo.pCode = reinterpret_cast<const uint32_t*>(fragShaderCode.data());

			if (vkCreateShaderModule(device, &createInfo, nullptr, &fragShaderModule) != VK_SUCCESS) {
				assert(false);
				exit(1);
			}

			VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
			fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderStageInfo.module = fragShaderModule;
			fragShaderStageInfo.pName = "main";

			shaderStages.push_back(fragShaderStageInfo);
		}


		VkGraphicsPipelineCreateInfo pipelineCreateInfo;
		pipelineCreateInfo.pVertexInputState = &emptyInputState;
		pipelineCreateInfo.pStages = shaderStages.data();

		vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, fullscreenPipeline);

		//VkCommandBuffer buff;// INITIALIZE BUFFER
		//vkCmdBindPipeline(buff, VK_PIPELINE_BIND_POINT_GRAPHICS, fullscreenPipeline);
		//vkCmdDraw(buff, 3, 1, 0, 0);
	}


	/**
	* \brief Set the danymic pipeline stat, i.e. the blend constants to be used
	*
	* \param[in] commandBuffer The currently used command buffer
	* \param[in] numPass The current pass number - in the forst pass, write over pixel colors, after this add pixel colors
	*
	*/
	void VESubrenderFW_Glow::setDynamicPipelineState(VkCommandBuffer commandBuffer, uint32_t numPass) {
		if (numPass == 0) {
			float blendConstants[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			vkCmdSetBlendConstants(commandBuffer, blendConstants);
			return;
		}

		float blendConstants[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		vkCmdSetBlendConstants(commandBuffer, blendConstants);
	}


	/**
	* \brief Add an entity to the subrenderer
	*
	* Create a UBO for this entity, a descriptor set per swapchain image, and update the descriptor sets
	*
	*/
	void VESubrenderFW_Glow::addEntity(VEEntity* pEntity) {

		std::vector<VkDescriptorImageInfo> maps = {
			pEntity->m_pMaterial->mapDiffuse->m_imageInfo,
			pEntity->m_pMaterial->mapNormal->m_imageInfo
		};

		addMaps(pEntity, maps);

		VESubrender::addEntity(pEntity);

	}

}


