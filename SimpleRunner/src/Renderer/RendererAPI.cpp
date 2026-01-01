#include "Renderer/RendererAPI.hpp"
#include "Core/Application.hpp"
#include "Core/Core.hpp"
#include "Log/Log.hpp"
#include "nvrhi/nvrhi.h"
#include <vulkan/vulkan_core.h>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#include "nvrhi/vulkan.h"
#include "nvrhi/validation.h"
#include "nvrhi/utils.h"

#include <GLFW/glfw3.h>

namespace SR
{

RendererContext RendererAPI::s_context;
SwapChain RendererAPI::s_swapChain;

void RendererAPI::VulkanInitValidationLayer()
{
#ifdef SR_DEBUG
    s_context.ValidationLayers.emplace_back("VK_LAYER_KHRONOS_validation");
	s_context.EnableValidationLayers = true;
#else
	s_context.EnableValidationLayers = false;
#endif
}

bool RendererAPI::VulkanCheckValidationLayerSupport()
{
	uint32_t availableLayerCount;
	vkEnumerateInstanceLayerProperties(&availableLayerCount, NULL);

    std::vector<VkLayerProperties> availableLayers(availableLayerCount);
	vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

	bool sucess = true;
	bool layerFound;
	uint32_t j;

	for (uint32_t i = 0; i < s_context.ValidationLayers.size(); i++)
    {
		layerFound = false;

		for (j = 0; j < availableLayerCount; j++) {
			if (strcmp(s_context.ValidationLayers[i], availableLayers[j].layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
		{
			sucess = false;
			break;
		}
	}

	CORE_LOG_SUCCESS("   == Vulkan Validation Layers Supported ==");
	for (uint32_t i = 0; i < availableLayerCount; i++)
    {
		CORE_LOG_SUCCESS("   - NAME   %s", (const char*)availableLayers[i].layerName);
	}

	if (!sucess)
	{
		CORE_LOG_ERROR("   == Vulkan Validation Layers not found  ==");
	}

	return sucess;
}

void RendererMessageCallback::message(nvrhi::MessageSeverity severity, const char* messageText)
{
    switch (severity)
    {
    case nvrhi::MessageSeverity::Info:
    {
        CORE_LOG_DEBUG("INFO: %s", messageText);
        break;
    }
    case nvrhi::MessageSeverity::Warning:
    {
        CORE_LOG_WARNING("WARNING: %s", messageText);
        break;
    }
    case nvrhi::MessageSeverity::Error:
    {
        CORE_LOG_ERROR("ERROR: %s", messageText);
        break;
    }
    case nvrhi::MessageSeverity::Fatal:
    {
        CORE_LOG_ERROR("FATAL: %s", messageText);
        break;
    }
    }
}

void RendererAPI::Init()
{
    if(!glfwVulkanSupported())
    {
        CORE_LOG_ERROR("GLFW reports that vulkan is not supported");
        return;
    }

    const uint32_t minimumVulkanVersion = VK_MAKE_API_VERSION(0, 1, 4, 0);

    VulkanInitValidationLayer();
    CreateInstance();

    // Check if the Vulkan API version is sufficient.
    const auto& version = vk::enumerateInstanceVersion();
    if (version < minimumVulkanVersion)
    {
        CORE_LOG_ERROR("The Vulkan API version supported on the system (%d.%d.%d) is too low, at least %d.%d.%d is required.",
                       VK_API_VERSION_MAJOR(version), VK_API_VERSION_MINOR(version), VK_API_VERSION_PATCH(version),
                       VK_API_VERSION_MAJOR(minimumVulkanVersion), VK_API_VERSION_MINOR(minimumVulkanVersion), VK_API_VERSION_PATCH(minimumVulkanVersion));
        return;
    }

    GLFWCreateSurface();
    s_context.PhysicalDevice = FindPhysicalDevice();
    s_context.Device = CreateDevice();

    nvrhi::vulkan::DeviceDesc deviceDesc;
    deviceDesc.errorCB = &s_context.ErrorCallback;
    deviceDesc.physicalDevice = s_context.PhysicalDevice;
    deviceDesc.device = s_context.Device;
    deviceDesc.graphicsQueue = s_context.GraphicsQueue;
    deviceDesc.computeQueue = nullptr;
    deviceDesc.transferQueue = nullptr;
    deviceDesc.graphicsQueueIndex = s_context.GraphicsQueueIndex;
    deviceDesc.deviceExtensions = s_context.PhysicalDeviceExtensions.data();
    deviceDesc.numDeviceExtensions = s_context.PhysicalDeviceExtensions.size();

    s_context.SetHandle(nvrhi::vulkan::createDevice(deviceDesc));
    if(s_context.EnableValidationLayers)
    {
        s_context.SetValidationHandle(nvrhi::validation::createValidationLayer(s_context.GetHandle()));
    }
    s_context.IsContextDeviceInitialized = true;

    s_swapChain.Init(s_context);

    CORE_LOG_SUCCESS("NVRHI vulkan initialized");
}

void RendererAPI::Shutdown()
{
    s_swapChain.Shutdown(s_context);

    s_context.SetValidationHandle(nullptr);
    s_context.SetHandle(nullptr);

    DestroyDevice();
    GLFWDestroySurface();
    DestroyInstance();


    s_context.PhysicalDevice = nullptr;
    RendererMessageCallback ErrorCallback;
    s_context.Instance = nullptr;
    s_context.Surface = nullptr;

    s_context.Device = nullptr;
    s_context.GraphicsQueue = nullptr;
    s_context.PresentQueue = nullptr;

    CORE_LOG_SUCCESS("RendererAPI has been shutdown");
}

void RendererAPI::BeginFrame(CommandList& commandList, const nvrhi::Color& clearColor)
{
    if(!s_swapChain.BeginFrame(commandList, s_context))
    {
        CORE_LOG_ERROR("vulkan renderer failed to begin frame");
    }

    commandList.GetHandle()->open();
    nvrhi::utils::ClearColorAttachment(
        commandList.GetHandle(), s_swapChain.GetFrameBufferInFlight(), 0, clearColor
    );
}

void RendererAPI::EndFrame(CommandList& commandList)
{
    commandList.GetHandle()->close();
    ExecuteCommandList(commandList);
    s_swapChain.EndFrame(s_context);
}

void RendererAPI::Present()
{
    if(!s_swapChain.Present(s_context))
    {
        CORE_LOG_ERROR("vulkan renderer failed to present frame");
    }
}

vk::PhysicalDevice RendererAPI::FindPhysicalDevice()
{
	//List all physical devices
    auto physicalDevices = s_context.Instance.enumeratePhysicalDevices();
	CORE_ASSERT(physicalDevices.size() > 0, "Failed to find GPUs with Vulkan support!");

	/*------------------ Search for physical device ------------------*/
	for (uint32_t i = 0; i < physicalDevices.size(); i++)
	{
		if (TryLoadingPhysicalDevice(physicalDevices[i]))
        {
			break;
		}
	}

	/*------------------ Show Total Search Results ------------------*/
	CORE_LOG_SUCCESS("   == Vulkan GPU available ==");
	for (size_t i = 0; i < physicalDevices.size(); i++)
	{
        vk::PhysicalDeviceProperties properties = physicalDevices[i].getProperties();
		CORE_LOG_SUCCESS("   - NAME   %s", properties.deviceName);
	}

	CORE_ASSERT(s_context.PhysicalDevice != VK_NULL_HANDLE, "Failed to find a suitable GPU!");

	/*------------------ Show Selected Results ------------------*/
	CORE_LOG_SUCCESS("   == Vulkan Chosen GPU ==");
	CORE_LOG_SUCCESS("   - NAME   %s", s_context.PhysicalDeviceProperties.deviceName);

	CORE_LOG_SUCCESS("Vulkan physical device selected");
    return s_context.PhysicalDevice;
}

bool RendererAPI::TryLoadingPhysicalDevice(vk::PhysicalDevice device)
{
	/*--------------------- Check For Physical Device Support ---------------------*/
    vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
	vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();
	vk::PhysicalDeviceMemoryProperties deviceMemoryProperties = device.getMemoryProperties();

	//base reuirement
	if (!deviceFeatures.samplerAnisotropy)
	{
		CORE_LOG_DEBUG("GPU [%s] does not support sampler anisotropy, skipping...", deviceProperties.deviceName);
		return false;
	}

	bool supportsDeviceLocalHostVisible = false;
	// check if device support local/host visible
	for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlags::BitsType::eHostVisible) &&
			(deviceMemoryProperties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlags::BitsType::eDeviceLocal))
		{
			supportsDeviceLocalHostVisible = true;
			break;
		}
	}

	if (supportsDeviceLocalHostVisible)
	{
		CORE_LOG_DEBUG("GPU [%s] support Local Host Visible", deviceProperties.deviceName);
	}

	/*--------------------- Cache Current GPU ---------------------*/
	s_context.PhysicalDevice = device;
	s_context.PhysicalDeviceProperties = deviceProperties;
	s_context.PhysicalDeviceFeature = deviceFeatures;
	s_context.PhysicalDeviceMemoryProperties = deviceMemoryProperties;
	s_context.SupportsDeviceLocalHostVisible = supportsDeviceLocalHostVisible;

	/*--------------------- Check For Queues Family ---------------------*/
	LoadQueueFamilies(device);
	//Bool extensionsSupported = VulkanSelectLogicalDevice(vkInfo, device);

	//Bool swapChainAdequate = false;
	//FE_VulkanfeSwapChainSupportDetails swapChainSupport = VulkanSwapChainSupportByQuery(vkInfo, device);
	//swapChainAdequate = !(swapChainSupport.formats.impl.count == 0) && !(swapChainSupport.presentModes.impl.count == 0);
	//VulkanClearSwapChainSupport(&swapChainSupport);

	return s_context.GraphicsFamily.has_value() && s_context.PresentFamily.has_value();//&& /*extensionsSupported && */ swapChainAdequate;
}

void RendererAPI::LoadQueueFamilies(vk::PhysicalDevice device)
{
    const auto& queueFamilies = device.getQueueFamilyProperties();

    uint32_t graphicsFamily = 0;
    uint32_t presentFamily = 0;
	for (uint32_t i = 0; i < queueFamilies.size(); i++)
	{
        bool graphicsBitSupport = static_cast<uint32_t>(queueFamilies[i].queueFlags & vk::QueueFlags::BitsType::eGraphics);
		if (graphicsBitSupport)
		{
			graphicsFamily = i;
            CORE_LOG_DEBUG("device: %s, graphics supported id: %zu", device.getProperties().deviceName.data(), i);
		}

        bool presentSupport = device.getSurfaceSupportKHR(i, s_context.Surface);

		if (presentSupport)
        {
			presentFamily = i;
            CORE_LOG_DEBUG("device: %s, present supported id: %zu", device.getProperties().deviceName.data(), i);
		}

		if (graphicsBitSupport && presentSupport)
		{
            s_context.GraphicsFamily = i;
            s_context.PresentFamily = i;
			break;
		}
	}
}

vk::Device RendererAPI::CreateDevice()
{
    /*--------------------- Check For Device Extension Support ---------------------*/

	// uint32_t extensionCount = 0;
	// vkEnumerateDeviceExtensionProperties(s_context.PhysicalDevice, NULL, &extensionCount, NULL);
    auto extensions = s_context.PhysicalDevice.enumerateDeviceExtensionProperties();

    s_context.PhysicalDeviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    // for(const auto& extension : extensions)
    // {
    //     if (strcmp(extension.extensionName, "VK_KHR_portability_subset") == 0)
    //     {
    //         CORE_LOG_DEBUG("Adding required device extension 'VK_KHR_portability_subset'");
    //         s_context.PhysicalDeviceExtensions.emplace_back("VK_KHR_portability_subset");
    //     }
    // }

	/*--------------------- Device Feature and Dynamic rendering features ---------------------*/
    vk::PhysicalDeviceFeatures2 deviceFeature{};
    deviceFeature.features = (VkPhysicalDeviceFeatures){
        .fillModeNonSolid = s_context.PhysicalDeviceFeature.fillModeNonSolid,
        .samplerAnisotropy = s_context.PhysicalDeviceFeature.samplerAnisotropy,
        .shaderClipDistance = s_context.PhysicalDeviceFeature.shaderClipDistance,
    };

	if (!deviceFeature.features.shaderClipDistance)
	{
		CORE_LOG_ERROR("ShaderClipDistance not supported by vulkan device '%s'", s_context.PhysicalDeviceProperties.deviceName);
	}

	// requiredDeviceExtensionsNames.emplace_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);

	/*--------------------- Find Queue Families ---------------------*/

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

	std::vector<uint32_t> uniqueQueueFamilies = {
        s_context.GraphicsFamily.value(),
        s_context.PresentFamily.value()
    };

    uniqueQueueFamilies.erase(unique(uniqueQueueFamilies.begin(), uniqueQueueFamilies.end()));

	if (uniqueQueueFamilies.size() > 1)
	{
        std::sort(uniqueQueueFamilies.begin(), uniqueQueueFamilies.end());
	}

	float queuePriority = 1.0f;

	queueCreateInfos.reserve(uniqueQueueFamilies.size());

	for (uint32_t i = 0; i < uniqueQueueFamilies.size(); i++)
	{
        vk::DeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.pNext = VK_NULL_HANDLE;
        queueCreateInfo.flags = static_cast<vk::DeviceQueueCreateFlags>(0);
        queueCreateInfo.queueFamilyIndex = uniqueQueueFamilies[i];
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	/*--------------------- Fill Create infos ---------------------*/
    vk::PhysicalDeviceFeatures2 physicalDeviceFeatures2;
    auto bufferDeviceAddressFeature = vk::PhysicalDeviceBufferDeviceAddressFeatures();
    bufferDeviceAddressFeature.pNext = physicalDeviceFeatures2;
    s_context.PhysicalDevice.getFeatures2(&physicalDeviceFeatures2);

    CORE_LOG_SUCCESS("   == Vulkan Device Extensions Supported ==");
    bool synchronization2Supported = false;
    for(auto extension : extensions)
    {
        // CORE_LOG_SUCCESS("   - NAME   %s || VERSION   %d", extension.extensionName, extension.specVersion);
        if(std::strcmp(extension.extensionName, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME) == 0)
        {
            synchronization2Supported = true;
        }
    }

    // Add a Vulkan 1.1 structure with default settings to make it easier for apps to modify them
    vk::PhysicalDeviceVulkan11Features vulkan11features = vk::PhysicalDeviceVulkan11Features()
        .setStorageBuffer16BitAccess(true)
        .setPNext(nullptr);

    vk::PhysicalDeviceVulkan12Features vulkan12features = vk::PhysicalDeviceVulkan12Features()
        .setDescriptorIndexing(true)
        .setRuntimeDescriptorArray(true)
        .setDescriptorBindingPartiallyBound(true)
        .setDescriptorBindingVariableDescriptorCount(true)
        .setTimelineSemaphore(true)
        .setShaderSampledImageArrayNonUniformIndexing(true)
        .setBufferDeviceAddress(bufferDeviceAddressFeature.bufferDeviceAddress)
        .setShaderSubgroupExtendedTypes(true)
        .setScalarBlockLayout(true)
        .setPNext(&vulkan11features);

    vk::PhysicalDeviceVulkan13Features vulkan13features = vk::PhysicalDeviceVulkan13Features()
        .setDynamicRendering(true)
        .setSynchronization2(synchronization2Supported)
        .setPNext(&vulkan12features);

    vulkan13features.pNext = &vulkan12features;
    deviceFeature.pNext = &vulkan13features;

    vk::DeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.pNext = &deviceFeature;
    deviceCreateInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.enabledExtensionCount = (uint32_t)s_context.PhysicalDeviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = s_context.PhysicalDeviceExtensions.data();

	//    vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo{ };
	// if (s_context.EnableValidationLayers)
	// {
	// 	deviceCreateInfo.enabledLayerCount = (uint32_t)s_context.ValidationLayers.size();
	// 	deviceCreateInfo.ppEnabledLayerNames = s_context.ValidationLayers.data();
	//        VulkanPopulateDebugMessenger(debugCreateInfo);
	//        deviceCreateInfo.pNext = &debugCreateInfo;
	// }
	// else
	// {
	// 	deviceCreateInfo.enabledLayerCount = 0;
	// }

	//create device based on pyshical device and deviceCreateInfo
    vk::Result result = s_context.PhysicalDevice.createDevice(&deviceCreateInfo, nullptr, &s_context.Device);
	CORE_ASSERT(result == vk::Result::eSuccess, "failed to create logical device!");

	/*--------------------- Store Queues ---------------------*/
    s_context.Device.getQueue(s_context.GraphicsFamily.value(), 0, &s_context.GraphicsQueue);
    s_context.Device.getQueue(s_context.PresentFamily.value(), 0, &s_context.PresentQueue);

	CORE_LOG_SUCCESS("Vulkan logical device created");

    VULKAN_HPP_DEFAULT_DISPATCHER.init(s_context.Device);

    return s_context.Device;
}

void RendererAPI::DestroyDevice()
{
    s_context.Device.destroy();
    CORE_LOG_SUCCESS("renderer device destroyed");
}

static void VulkanShowDebug(
	vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData)
{
	switch (messageSeverity)
	{
	case vk::DebugUtilsMessageSeverityFlagsEXT::BitsType::eError:
		CORE_LOG_ERROR("Vulkan Validation layer: %s", pCallbackData->pMessage);
		return;
	case vk::DebugUtilsMessageSeverityFlagsEXT::BitsType::eWarning:
		CORE_LOG_WARNING("Vulkan Validation layer: %s", pCallbackData->pMessage);
		return;
	case vk::DebugUtilsMessageSeverityFlagsEXT::BitsType::eInfo:
		CORE_LOG_DEBUG("Vulkan Validation layer: %s", pCallbackData->pMessage);
		return;
	case vk::DebugUtilsMessageSeverityFlagsEXT::BitsType::eVerbose:
		CORE_LOG_DEBUG("Vulkan Validation layer: %s", pCallbackData->pMessage);
		return;
	default:
		CORE_LOG_DEBUG("Vulkan Validation layer: %s", pCallbackData->pMessage);
		return;
	}
}

static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
	vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	vk::DebugUtilsMessageTypeFlagsEXT messageType,
	const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
    RendererContext& context = *static_cast<RendererContext*>(pUserData);
	if (context.EnableValidationLayers || messageSeverity >= vk::DebugUtilsMessageSeverityFlagsEXT::BitsType::eWarning)
	{
		VulkanShowDebug(messageSeverity, pCallbackData);
	}

	return VK_FALSE;
}

void RendererAPI::VulkanPopulateDebugMessenger(vk::DebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagsEXT::BitsType::eInfo |
                                 vk::DebugUtilsMessageSeverityFlagsEXT::BitsType::eVerbose |
                                 vk::DebugUtilsMessageSeverityFlagsEXT::BitsType::eWarning |
                                 vk::DebugUtilsMessageSeverityFlagsEXT::BitsType::eError;

    createInfo.messageType = vk::DebugUtilsMessageTypeFlagsEXT::BitsType::eGeneral |
                             vk::DebugUtilsMessageTypeFlagsEXT::BitsType::eValidation |
                             vk::DebugUtilsMessageTypeFlagsEXT::BitsType::ePerformance;

	createInfo.pfnUserCallback = VulkanDebugCallback;
	createInfo.pUserData = &s_context;
}

void RendererAPI::CreateInstance()
{
    VULKAN_HPP_DEFAULT_DISPATCHER.init();
	//Init validation layers
	if (s_context.EnableValidationLayers && !VulkanCheckValidationLayerSupport())
	{
		s_context.ValidationLayers.clear();
		CORE_LOG_ERROR("Vulkan validation layers are enabled but not available. Therefore disabling validation layers");
		s_context.EnableValidationLayers = false;
	}

	//Create instance data
    vk::ApplicationInfo appInfo = vk::ApplicationInfo()
        .setApiVersion(VK_API_VERSION_1_4)
        .setApplicationVersion(VK_MAKE_VERSION(1, 4, 0))
        .setEngineVersion(VK_MAKE_VERSION(1, 4, 0))
        .setPApplicationName("Vulkan Simple Runner")
        .setPEngineName("Simple Runner Engine");

    vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo()
        .setPApplicationInfo(&appInfo);
#ifdef SR_PLATFORM_MACOS
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

	//setup validation layers/window extensions into the instance data
    vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo{ };
	if (s_context.EnableValidationLayers)
	{
		createInfo.enabledLayerCount = (uint32_t)s_context.ValidationLayers.size();
		createInfo.ppEnabledLayerNames = s_context.ValidationLayers.data();
        VulkanPopulateDebugMessenger(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
        createInfo.pNext = nullptr;
	}

	//window extension
    std::vector<const char*> createInfoExtensions;

#ifdef SR_PLATFORM_MACOS
	FE_ListPushValue(createInfoExtensions, const char* const, "VK_KHR_portability_enumeration");
#endif

	if (s_context.EnableValidationLayers)
    {
		createInfoExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

    GLFWSetExtension(&createInfo, createInfoExtensions);

    CORE_LOG_SUCCESS("   == Vulkan Window Extensions Supported ==");

    for (const auto& extension : vk::enumerateInstanceExtensionProperties())
    {
        CORE_LOG_SUCCESS("   - NAME   %s || VERSION   %d", (const char*)extension.extensionName, extension.specVersion);
    }

	//create instance
    vk::Result result = vk::createInstance(&createInfo, NULL, &s_context.Instance);
	CORE_ASSERT(result == vk::Result::eSuccess, "failed to create vulkan instance");

	CORE_LOG_SUCCESS("Vulkan instance created");

    VULKAN_HPP_DEFAULT_DISPATCHER.init(s_context.Instance);
}

void RendererAPI::DestroyInstance()
{
    s_context.Instance.destroy();
    CORE_LOG_SUCCESS("renderer instance destroyed");
}

void RendererAPI::GLFWCreateSurface()
{
    VkSurfaceKHR surface;
    VkResult result = glfwCreateWindowSurface(
        s_context.Instance,
        static_cast<GLFWwindow*>(Application::GetApp().GetMainWindow().GetNative()),
        nullptr,
        &surface
    );
    CORE_ASSERT_PARAMS(result == VK_SUCCESS, "failed to create surface, code: %d", result);

    s_context.Surface = surface;
    CORE_LOG_SUCCESS("GLFW Surface created");
}

void RendererAPI::GLFWDestroySurface()
{
    s_context.Instance.destroySurfaceKHR(s_context.Surface);
    CORE_LOG_SUCCESS("glfw surface destroyed");
}

void RendererAPI::GLFWSetExtension(vk::InstanceCreateInfo* createInfo, std::vector<const char*>&  extensions)
{
	uint32_t glfwExtensionCount = 0;
	const char* const* glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	CORE_LOG_SUCCESS("   == Vulkan GLFW required extensions count: %zu ==", glfwExtensionCount);
    for(uint32_t i = 0; i < glfwExtensionCount; ++i)
    {
        CORE_LOG_SUCCESS("   - NAME   %s", glfwExtensions[i]);
        extensions.emplace_back(glfwExtensions[i]);
    }

	CORE_LOG_SUCCESS("   == Vulkan final GLFW extensions ==");
	for (size_t i = 0; i < extensions.size(); i++)
	{
		CORE_LOG_SUCCESS("   - NAME   %s", extensions[i]);
	}

	createInfo->enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo->ppEnabledExtensionNames = extensions.data();
}

}
