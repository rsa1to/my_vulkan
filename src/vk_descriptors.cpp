#include <vk_descriptors.h>

void DescriptorLayoutBuilder::add_binding(uint32_t binding, VkDescriptorType type) {
	VkDescriptorSetLayoutBinding newBind {};
	newBind.binding = binding;
	newBind.descriptorType = type;
	newBind.descriptorCount = 1;

	bindings.push_back(newBind);
}

void DescriptorLayoutBuilder::clear() {
	bindings.clear();
}

VkDescriptorSetLayout DescriptorLayoutBuilder::build(VkDevice device, VkShaderStageFlags shaderStages, void* pNext, VkDescriptorSetLayoutCreateFlags flags) {
	for(auto& b: bindings) {
		b.stageFlags = shaderStages;
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
	layoutInfo.pNext = pNext,
	layoutInfo.pBindings = bindings.data(),
	layoutInfo.bindingCount = (uint32_t)bindings.size(),
	layoutInfo.flags = flags;

	VkDescriptorSetLayout layout;
	VK_CHECK(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout));
	return layout;
}

void DescriptorAllocator::init_pool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios) {
	std::vector<VkDescriptorPoolSize> poolSizes;
	for (PoolSizeRatio r: poolRatios) {
		poolSizes.push_back({ .type = r.type, .descriptorCount = (uint32_t)(maxSets * r.ratio) });
	}

	VkDescriptorPoolCreateInfo poolInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
	poolInfo.flags = 0;
	poolInfo.maxSets = maxSets;
	poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
	poolInfo.pPoolSizes = poolSizes.data();

	VK_CHECK(vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool));
}

void DescriptorAllocator::clear_descriptors(VkDevice device) {
	vkResetDescriptorPool(device, pool, 0);
}

void DescriptorAllocator::destroy_pool(VkDevice device) {
	vkDestroyDescriptorPool(device, pool, nullptr);
}

VkDescriptorSet DescriptorAllocator::allocate(VkDevice device, VkDescriptorSetLayout layout) {
	VkDescriptorSetAllocateInfo allocInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
	allocInfo.pNext = nullptr;
	allocInfo.descriptorPool = pool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &layout;

	VkDescriptorSet set;
	VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, &set));
	return set;
}