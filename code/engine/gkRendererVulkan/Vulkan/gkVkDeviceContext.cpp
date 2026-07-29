#include "gkVkDeviceContext.h"

#include "ISystem.h"
#include "IMesh.h"

#include <vulkan/vulkan.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <limits>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

namespace
{
	const uint32 kFramesInFlight = 2;
	const uint32 kTextureStageCount = 16;
	const TCHAR* kWindowClassName = _T("gkVulkanWindowClass");

	bool hasName(const std::vector<VkExtensionProperties>& properties, const char* name)
	{
		for (size_t i = 0; i < properties.size(); ++i)
			if (strcmp(properties[i].extensionName, name) == 0)
				return true;
		return false;
	}

	bool hasLayer(const char* name)
	{
		uint32 count = 0;
		vkEnumerateInstanceLayerProperties(&count, NULL);
		std::vector<VkLayerProperties> layers(count);
		if (count)
			vkEnumerateInstanceLayerProperties(&count, &layers[0]);
		for (uint32 i = 0; i < count; ++i)
			if (strcmp(layers[i].layerName, name) == 0)
				return true;
		return false;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		VkDebugUtilsMessageTypeFlagsEXT,
		const VkDebugUtilsMessengerCallbackDataEXT* data,
		void*)
	{
		if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			fprintf(stderr, "[gkVulkan] %s\n",
				data && data->pMessage ? data->pMessage : "validation message");
			OutputDebugStringA("[gkVulkan] ");
			OutputDebugStringA(data && data->pMessage ? data->pMessage : "validation message");
			OutputDebugStringA("\n");
		}
		return VK_FALSE;
	}
}

struct gkVkDeviceContext::Impl
{
	struct GpuTexture
	{
		VkImage image;
		VkDeviceMemory memory;
		VkImageView view;
		uint32 width;
		uint32 height;
		uint64 revision;

		GpuTexture()
			: image(VK_NULL_HANDLE), memory(VK_NULL_HANDLE), view(VK_NULL_HANDLE)
			, width(0), height(0), revision(0) {}
	};

	struct GpuBuffer
	{
		VkBuffer buffer;
		VkDeviceMemory memory;
		VkDeviceSize size;

		GpuBuffer() : buffer(VK_NULL_HANDLE), memory(VK_NULL_HANDLE), size(0) {}
	};

	struct FrameContext
	{
		struct TransientBuffer
		{
			VkBuffer buffer;
			VkDeviceMemory memory;
		};

		VkCommandPool commandPool;
		VkCommandBuffer commandBuffer;
		VkSemaphore imageAvailable;
		VkFence fence;
		VkQueryPool timestampPool;
		bool timestampsIssued;
		std::vector<TransientBuffer> transientBuffers;

		FrameContext()
			: commandPool(VK_NULL_HANDLE)
			, commandBuffer(VK_NULL_HANDLE)
			, imageAvailable(VK_NULL_HANDLE)
			, fence(VK_NULL_HANDLE)
			, timestampPool(VK_NULL_HANDLE)
			, timestampsIssued(false)
		{
		}
	};

	HINSTANCE hinstance;
	HWND hwnd;
	bool ownsWindow;
	bool windowClassRegistered;
	uint32 requestedWidth;
	uint32 requestedHeight;
	bool resizePending;
	bool isMinimized;
	bool isFullscreen;
	DWORD windowedStyle;
	RECT windowedRect;

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	uint32 graphicsFamily;
	uint32 presentFamily;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkPipelineCache pipelineCache;
	uint32 physicalApiVersion;
	uint32 instanceApiVersion;
	bool samplerAnisotropySupported;
	float maxSamplerAnisotropy;
	bool timestampsSupported;
	uint32 timestampValidBits;
	float timestampPeriod;
	float lastGpuFrameMs;

	VkSwapchainKHR swapchain;
	VkFormat swapchainFormat;
	VkExtent2D swapchainExtent;
	VkPipelineLayout acceptancePipelineLayout;
	VkPipeline acceptancePipeline;
	VkPipelineLayout scenePipelineLayout;
	VkPipeline scenePipelines[5][5][2];
	VkDescriptorSetLayout sceneDescriptorSetLayout;
	VkDescriptorPool sceneDescriptorPool;
	VkSampler sceneSampler;
	std::unordered_map<const ITexture*, GpuTexture> gpuTextures;
	std::map<std::vector<const ITexture*>, VkDescriptorSet> textureDescriptorSets;
	std::unordered_map<const gkVertexBuffer*, GpuBuffer> gpuVertexBuffers;
	std::unordered_map<const gkIndexBuffer*, GpuBuffer> gpuIndexBuffers;
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;
	std::vector<bool> imageInitialized;
	std::vector<VkSemaphore> presentSemaphores;
	VkFormat depthFormat;
	std::vector<VkImage> depthImages;
	std::vector<VkDeviceMemory> depthMemories;
	std::vector<VkImageView> depthViews;
	std::vector<bool> depthInitialized;

	FrameContext frames[kFramesInFlight];
	uint32 frameIndex;
	uint32 acquiredImage;
	bool frameActive;
	bool renderingActive;
	uint32 frameDrawCount;

	PFN_vkCreateDebugUtilsMessengerEXT createDebugMessenger;
	PFN_vkDestroyDebugUtilsMessengerEXT destroyDebugMessenger;
	PFN_vkCmdBeginRenderingKHR cmdBeginRendering;
	PFN_vkCmdEndRenderingKHR cmdEndRendering;
	PFN_vkCmdPipelineBarrier2KHR cmdPipelineBarrier2;

	Impl()
		: hinstance(NULL)
		, hwnd(NULL)
		, ownsWindow(false)
		, windowClassRegistered(false)
		, requestedWidth(0)
		, requestedHeight(0)
		, resizePending(false)
		, isMinimized(false)
		, isFullscreen(false)
		, windowedStyle(WS_OVERLAPPEDWINDOW)
		, instance(VK_NULL_HANDLE)
		, debugMessenger(VK_NULL_HANDLE)
		, surface(VK_NULL_HANDLE)
		, physicalDevice(VK_NULL_HANDLE)
		, device(VK_NULL_HANDLE)
		, graphicsFamily(UINT32_MAX)
		, presentFamily(UINT32_MAX)
		, graphicsQueue(VK_NULL_HANDLE)
		, presentQueue(VK_NULL_HANDLE)
		, pipelineCache(VK_NULL_HANDLE)
		, physicalApiVersion(VK_API_VERSION_1_2)
		, instanceApiVersion(VK_API_VERSION_1_2)
		, samplerAnisotropySupported(false)
		, maxSamplerAnisotropy(1.0f)
		, timestampsSupported(false)
		, timestampValidBits(0)
		, timestampPeriod(1.0f)
		, lastGpuFrameMs(0.0f)
		, swapchain(VK_NULL_HANDLE)
		, swapchainFormat(VK_FORMAT_UNDEFINED)
		, depthFormat(VK_FORMAT_D32_SFLOAT)
		, acceptancePipelineLayout(VK_NULL_HANDLE)
		, acceptancePipeline(VK_NULL_HANDLE)
		, scenePipelineLayout(VK_NULL_HANDLE)
		, sceneDescriptorSetLayout(VK_NULL_HANDLE)
		, sceneDescriptorPool(VK_NULL_HANDLE)
		, sceneSampler(VK_NULL_HANDLE)
		, frameIndex(0)
		, acquiredImage(0)
		, frameActive(false)
		, renderingActive(false)
		, frameDrawCount(0)
		, createDebugMessenger(NULL)
		, destroyDebugMessenger(NULL)
		, cmdBeginRendering(NULL)
		, cmdEndRendering(NULL)
		, cmdPipelineBarrier2(NULL)
	{
		swapchainExtent.width = 0;
		swapchainExtent.height = 0;
		SetRectEmpty(&windowedRect);
		for (uint32 i = 0; i < 5; ++i)
			for (uint32 variant = 0; variant < 5; ++variant)
				for (uint32 sidedness = 0; sidedness < 2; ++sidedness)
					scenePipelines[i][variant][sidedness] = VK_NULL_HANDLE;
	}

	static LRESULT CALLBACK windowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
	{
		Impl* self = reinterpret_cast<Impl*>(GetWindowLongPtr(window, GWLP_USERDATA));
		if (message == WM_NCCREATE)
		{
			CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lparam);
			self = reinterpret_cast<Impl*>(create->lpCreateParams);
			SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
		}

		if (self)
		{
			switch (message)
			{
			case WM_SIZE:
				self->requestedWidth = static_cast<uint32>(LOWORD(lparam));
				self->requestedHeight = static_cast<uint32>(HIWORD(lparam));
				self->isMinimized = (wparam == SIZE_MINIMIZED) ||
					self->requestedWidth == 0 || self->requestedHeight == 0;
				self->resizePending = !self->isMinimized;
				return 0;
			case WM_ERASEBKGND:
				return 1;
			case WM_CLOSE:
				DestroyWindow(window);
				return 0;
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
			}
		}
		return DefWindowProc(window, message, wparam, lparam);
	}

	bool createWindow(const ISystemInitInfo& initInfo)
	{
		hinstance = GetModuleHandle(NULL);
		requestedWidth = initInfo.fWidth > 0 ? static_cast<uint32>(initInfo.fWidth) : 1280;
		requestedHeight = initInfo.fHeight > 0 ? static_cast<uint32>(initInfo.fHeight) : 720;

		WNDCLASSEX windowClass;
		ZeroMemory(&windowClass, sizeof(windowClass));
		windowClass.cbSize = sizeof(windowClass);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = windowProc;
		windowClass.hInstance = hinstance;
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.lpszClassName = kWindowClassName;

		ATOM atom = RegisterClassEx(&windowClass);
		if (!atom && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
			return false;
		windowClassRegistered = atom != 0;

		RECT rect = { 0, 0, static_cast<LONG>(requestedWidth), static_cast<LONG>(requestedHeight) };
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
		hwnd = CreateWindowEx(
			0, kWindowClassName, _T("gkENGINE Vulkan"), WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
			NULL, NULL, hinstance, this);
		if (!hwnd)
			return false;
		ownsWindow = true;
		ShowWindow(hwnd, SW_SHOW);
		UpdateWindow(hwnd);
		GetWindowRect(hwnd, &windowedRect);
		return true;
	}

	void setFullscreen(bool fullscreen)
	{
		if (!hwnd || !ownsWindow || fullscreen == isFullscreen)
			return;
		if (fullscreen)
		{
			windowedStyle = static_cast<DWORD>(GetWindowLongPtr(hwnd, GWL_STYLE));
			GetWindowRect(hwnd, &windowedRect);
			MONITORINFO monitor = {};
			monitor.cbSize = sizeof(monitor);
			GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &monitor);
			SetWindowLongPtr(hwnd, GWL_STYLE,
				(windowedStyle & ~WS_OVERLAPPEDWINDOW) | WS_POPUP);
			SetWindowPos(hwnd, HWND_TOP,
				monitor.rcMonitor.left, monitor.rcMonitor.top,
				monitor.rcMonitor.right - monitor.rcMonitor.left,
				monitor.rcMonitor.bottom - monitor.rcMonitor.top,
				SWP_FRAMECHANGED | SWP_SHOWWINDOW);
		}
		else
		{
			SetWindowLongPtr(hwnd, GWL_STYLE, windowedStyle);
			SetWindowPos(hwnd, HWND_NOTOPMOST,
				windowedRect.left, windowedRect.top,
				windowedRect.right - windowedRect.left,
				windowedRect.bottom - windowedRect.top,
				SWP_FRAMECHANGED | SWP_SHOWWINDOW);
		}
		isFullscreen = fullscreen;
	}

	bool createInstance()
	{
		uint32 loaderVersion = VK_API_VERSION_1_0;
		PFN_vkEnumerateInstanceVersion enumerateVersion =
			reinterpret_cast<PFN_vkEnumerateInstanceVersion>(
				vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceVersion"));
		if (enumerateVersion)
			enumerateVersion(&loaderVersion);
		if (loaderVersion < VK_API_VERSION_1_2)
			return false;
		instanceApiVersion = loaderVersion >= VK_API_VERSION_1_3 ?
			VK_API_VERSION_1_3 : VK_API_VERSION_1_2;

		uint32 extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
		std::vector<VkExtensionProperties> available(extensionCount);
		if (extensionCount)
			vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, &available[0]);

		const char* required[] = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
		for (size_t i = 0; i < sizeof(required) / sizeof(required[0]); ++i)
			if (!hasName(available, required[i]))
				return false;

		std::vector<const char*> extensions(required, required + 2);
		bool enableDebug = false;
#if defined(GK_VULKAN_VALIDATION)
		enableDebug = hasName(available, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		if (enableDebug)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

		std::vector<const char*> layers;
#if defined(GK_VULKAN_VALIDATION)
		if (hasLayer("VK_LAYER_KHRONOS_validation"))
			layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "gkENGINE";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "gkENGINE";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = instanceApiVersion;

		VkDebugUtilsMessengerCreateInfoEXT debugInfo = {};
		debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugInfo.pfnUserCallback = debugCallback;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.empty() ? NULL : &extensions[0];
		createInfo.enabledLayerCount = static_cast<uint32>(layers.size());
		createInfo.ppEnabledLayerNames = layers.empty() ? NULL : &layers[0];
		createInfo.pNext = enableDebug ? &debugInfo : NULL;

		if (vkCreateInstance(&createInfo, NULL, &instance) != VK_SUCCESS)
			return false;

		createDebugMessenger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
			vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
		destroyDebugMessenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
			vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
		if (enableDebug && createDebugMessenger)
			createDebugMessenger(instance, &debugInfo, NULL, &debugMessenger);
		return true;
	}

	bool createSurface()
	{
		VkWin32SurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hinstance = hinstance;
		createInfo.hwnd = hwnd;
		return vkCreateWin32SurfaceKHR(instance, &createInfo, NULL, &surface) == VK_SUCCESS;
	}

	bool deviceExtensionsSupported(VkPhysicalDevice candidate, uint32 apiVersion)
	{
		uint32 count = 0;
		vkEnumerateDeviceExtensionProperties(candidate, NULL, &count, NULL);
		std::vector<VkExtensionProperties> extensions(count);
		if (count)
			vkEnumerateDeviceExtensionProperties(candidate, NULL, &count, &extensions[0]);
		if (!hasName(extensions, VK_KHR_SWAPCHAIN_EXTENSION_NAME))
			return false;
		if (apiVersion < VK_API_VERSION_1_3)
			return hasName(extensions, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME) &&
				hasName(extensions, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);
		return true;
	}

	bool findQueueFamilies(VkPhysicalDevice candidate, uint32& graphics, uint32& present)
	{
		uint32 count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(candidate, &count, NULL);
		std::vector<VkQueueFamilyProperties> families(count);
		if (count)
			vkGetPhysicalDeviceQueueFamilyProperties(candidate, &count, &families[0]);
		graphics = UINT32_MAX;
		present = UINT32_MAX;
		for (uint32 i = 0; i < count; ++i)
		{
			if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				graphics = i;
			VkBool32 supportsPresent = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(candidate, i, surface, &supportsPresent);
			if (supportsPresent)
				present = i;
			if (graphics != UINT32_MAX && present != UINT32_MAX)
				return true;
		}
		return false;
	}

	bool hasSwapchainSupport(VkPhysicalDevice candidate)
	{
		uint32 formats = 0;
		uint32 modes = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(candidate, surface, &formats, NULL);
		vkGetPhysicalDeviceSurfacePresentModesKHR(candidate, surface, &modes, NULL);
		return formats > 0 && modes > 0;
	}

	bool selectPhysicalDevice()
	{
		uint32 count = 0;
		vkEnumeratePhysicalDevices(instance, &count, NULL);
		if (!count)
			return false;
		std::vector<VkPhysicalDevice> devices(count);
		vkEnumeratePhysicalDevices(instance, &count, &devices[0]);

		int bestScore = -1;
		for (uint32 i = 0; i < count; ++i)
		{
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(devices[i], &properties);
			uint32 graphics = UINT32_MAX;
			uint32 present = UINT32_MAX;
			const uint32 effectiveApiVersion = (std::min)(properties.apiVersion, instanceApiVersion);
			if (effectiveApiVersion < VK_API_VERSION_1_2 ||
				!findQueueFamilies(devices[i], graphics, present) ||
				!deviceExtensionsSupported(devices[i], effectiveApiVersion) ||
				!hasSwapchainSupport(devices[i]))
				continue;

			VkPhysicalDeviceDynamicRenderingFeatures dynamicFeatures = {};
			dynamicFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
			VkPhysicalDeviceSynchronization2Features syncFeatures = {};
			syncFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
			dynamicFeatures.pNext = &syncFeatures;
			VkPhysicalDeviceFeatures2 features = {};
			features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
			features.pNext = &dynamicFeatures;
			vkGetPhysicalDeviceFeatures2(devices[i], &features);
			if (!dynamicFeatures.dynamicRendering || !syncFeatures.synchronization2)
				continue;

			int score = properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 1000 : 100;
			score += static_cast<int>(properties.limits.maxImageDimension2D / 1024);
			if (score > bestScore)
			{
				bestScore = score;
				physicalDevice = devices[i];
				physicalApiVersion = properties.apiVersion;
				graphicsFamily = graphics;
				presentFamily = present;
				samplerAnisotropySupported =
					features.features.samplerAnisotropy == VK_TRUE;
				maxSamplerAnisotropy = properties.limits.maxSamplerAnisotropy;
			}
		}

		if (physicalDevice == VK_NULL_HANDLE)
			return false;

		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);
		timestampPeriod = properties.limits.timestampPeriod;
		uint32 familyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, NULL);
		std::vector<VkQueueFamilyProperties> familyProperties(familyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(
			physicalDevice, &familyCount, &familyProperties[0]);
		if (graphicsFamily < familyProperties.size())
		{
			timestampValidBits = familyProperties[graphicsFamily].timestampValidBits;
			timestampsSupported = timestampValidBits > 0;
		}
		char message[512];
		sprintf_s(message, "[gkVulkan] Selected GPU: %s (Vulkan %u.%u.%u)\n",
			properties.deviceName,
			VK_VERSION_MAJOR(properties.apiVersion),
			VK_VERSION_MINOR(properties.apiVersion),
			VK_VERSION_PATCH(properties.apiVersion));
		OutputDebugStringA(message);
		return true;
	}

	bool createDevice()
	{
		std::set<uint32> uniqueFamilies;
		uniqueFamilies.insert(graphicsFamily);
		uniqueFamilies.insert(presentFamily);
		float priority = 1.0f;
		std::vector<VkDeviceQueueCreateInfo> queues;
		for (std::set<uint32>::const_iterator it = uniqueFamilies.begin(); it != uniqueFamilies.end(); ++it)
		{
			VkDeviceQueueCreateInfo queue = {};
			queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue.queueFamilyIndex = *it;
			queue.queueCount = 1;
			queue.pQueuePriorities = &priority;
			queues.push_back(queue);
		}

		std::vector<const char*> extensions;
		extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		const uint32 effectiveApiVersion = (std::min)(physicalApiVersion, instanceApiVersion);
		if (effectiveApiVersion < VK_API_VERSION_1_3)
		{
			extensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
			extensions.push_back(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);
		}

		VkPhysicalDeviceSynchronization2Features syncFeatures = {};
		syncFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
		syncFeatures.synchronization2 = VK_TRUE;
		VkPhysicalDeviceDynamicRenderingFeatures dynamicFeatures = {};
		dynamicFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
		dynamicFeatures.dynamicRendering = VK_TRUE;
		dynamicFeatures.pNext = &syncFeatures;

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pNext = &dynamicFeatures;
		VkPhysicalDeviceFeatures enabledFeatures = {};
		enabledFeatures.samplerAnisotropy = samplerAnisotropySupported ? VK_TRUE : VK_FALSE;
		createInfo.pEnabledFeatures = &enabledFeatures;
		createInfo.queueCreateInfoCount = static_cast<uint32>(queues.size());
		createInfo.pQueueCreateInfos = &queues[0];
		createInfo.enabledExtensionCount = static_cast<uint32>(extensions.size());
		createInfo.ppEnabledExtensionNames = &extensions[0];
		if (vkCreateDevice(physicalDevice, &createInfo, NULL, &device) != VK_SUCCESS)
			return false;

		vkGetDeviceQueue(device, graphicsFamily, 0, &graphicsQueue);
		vkGetDeviceQueue(device, presentFamily, 0, &presentQueue);

		cmdBeginRendering = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(
			vkGetDeviceProcAddr(device, "vkCmdBeginRendering"));
		if (!cmdBeginRendering)
			cmdBeginRendering = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(
				vkGetDeviceProcAddr(device, "vkCmdBeginRenderingKHR"));
		cmdEndRendering = reinterpret_cast<PFN_vkCmdEndRenderingKHR>(
			vkGetDeviceProcAddr(device, "vkCmdEndRendering"));
		if (!cmdEndRendering)
			cmdEndRendering = reinterpret_cast<PFN_vkCmdEndRenderingKHR>(
				vkGetDeviceProcAddr(device, "vkCmdEndRenderingKHR"));
		cmdPipelineBarrier2 = reinterpret_cast<PFN_vkCmdPipelineBarrier2KHR>(
			vkGetDeviceProcAddr(device, "vkCmdPipelineBarrier2"));
		if (!cmdPipelineBarrier2)
			cmdPipelineBarrier2 = reinterpret_cast<PFN_vkCmdPipelineBarrier2KHR>(
				vkGetDeviceProcAddr(device, "vkCmdPipelineBarrier2KHR"));
		return cmdBeginRendering && cmdEndRendering && cmdPipelineBarrier2;
	}

	bool executableSiblingPath(const char* filename, char (&path)[MAX_PATH])
	{
		DWORD pathLength = GetModuleFileNameA(NULL, path, MAX_PATH);
		if (!pathLength || pathLength >= MAX_PATH)
			return false;
		char* slash = strrchr(path, '\\');
		if (!slash)
			return false;
		*(slash + 1) = '\0';
		return strcat_s(path, filename) == 0;
	}

	bool createPipelineCache()
	{
		std::vector<uint8> initialData;
		char path[MAX_PATH] = {};
		if (executableSiblingPath("gkVulkanPipelineCache.bin", path))
		{
			FILE* file = NULL;
			if (fopen_s(&file, path, "rb") == 0 && file)
			{
				fseek(file, 0, SEEK_END);
				const long size = ftell(file);
				fseek(file, 0, SEEK_SET);
				if (size > 0)
				{
					initialData.resize(static_cast<size_t>(size));
					if (fread(&initialData[0], 1, initialData.size(), file) != initialData.size())
						initialData.clear();
				}
				fclose(file);
			}
		}
		VkPipelineCacheCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		createInfo.initialDataSize = initialData.size();
		createInfo.pInitialData = initialData.empty() ? NULL : &initialData[0];
		VkResult result = vkCreatePipelineCache(device, &createInfo, NULL, &pipelineCache);
		if (result != VK_SUCCESS && !initialData.empty())
		{
			createInfo.initialDataSize = 0;
			createInfo.pInitialData = NULL;
			result = vkCreatePipelineCache(device, &createInfo, NULL, &pipelineCache);
		}
		return result == VK_SUCCESS;
	}

	void saveAndDestroyPipelineCache()
	{
		if (!pipelineCache)
			return;
		size_t size = 0;
		if (vkGetPipelineCacheData(device, pipelineCache, &size, NULL) == VK_SUCCESS && size)
		{
			std::vector<uint8> data(size);
			if (vkGetPipelineCacheData(device, pipelineCache, &size, &data[0]) == VK_SUCCESS)
			{
				char path[MAX_PATH] = {};
				if (executableSiblingPath("gkVulkanPipelineCache.bin", path))
				{
					FILE* file = NULL;
					if (fopen_s(&file, path, "wb") == 0 && file)
					{
						fwrite(&data[0], 1, size, file);
						fclose(file);
					}
				}
			}
		}
		vkDestroyPipelineCache(device, pipelineCache, NULL);
		pipelineCache = VK_NULL_HANDLE;
	}

	VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
	{
		for (size_t i = 0; i < formats.size(); ++i)
			if (formats[i].format == VK_FORMAT_B8G8R8A8_UNORM &&
				formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return formats[i];
		for (size_t i = 0; i < formats.size(); ++i)
			if (formats[i].format == VK_FORMAT_R8G8B8A8_UNORM)
				return formats[i];
		return formats[0];
	}

	bool createSwapchain()
	{
		VkSurfaceCapabilitiesKHR capabilities;
		if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities) != VK_SUCCESS)
			return false;

		uint32 formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL);
		std::vector<VkSurfaceFormatKHR> formats(formatCount);
		if (formatCount)
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, &formats[0]);
		if (formats.empty())
			return false;

		uint32 modeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &modeCount, NULL);
		std::vector<VkPresentModeKHR> modes(modeCount);
		if (modeCount)
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &modeCount, &modes[0]);

		VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(formats);
		VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

		if (capabilities.currentExtent.width != std::numeric_limits<uint32>::max())
			swapchainExtent = capabilities.currentExtent;
		else
		{
			swapchainExtent.width = (std::max)(capabilities.minImageExtent.width,
				(std::min)(capabilities.maxImageExtent.width, requestedWidth));
			swapchainExtent.height = (std::max)(capabilities.minImageExtent.height,
				(std::min)(capabilities.maxImageExtent.height, requestedHeight));
		}
		if (swapchainExtent.width == 0 || swapchainExtent.height == 0)
			return false;

		uint32 imageCount = capabilities.minImageCount + 1;
		if (capabilities.maxImageCount > 0)
			imageCount = (std::min)(imageCount, capabilities.maxImageCount);

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = swapchainExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		uint32 queueFamilies[] = { graphicsFamily, presentFamily };
		if (graphicsFamily != presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilies;
		}
		else
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.preTransform = capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;
		if (vkCreateSwapchainKHR(device, &createInfo, NULL, &swapchain) != VK_SUCCESS)
			return false;

		swapchainFormat = surfaceFormat.format;
		vkGetSwapchainImagesKHR(device, swapchain, &imageCount, NULL);
		images.resize(imageCount);
		vkGetSwapchainImagesKHR(device, swapchain, &imageCount, &images[0]);
		imageViews.resize(imageCount, VK_NULL_HANDLE);
		imageInitialized.resize(imageCount, false);
		presentSemaphores.resize(imageCount, VK_NULL_HANDLE);
		for (uint32 i = 0; i < imageCount; ++i)
		{
			VkImageViewCreateInfo view = {};
			view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			view.image = images[i];
			view.viewType = VK_IMAGE_VIEW_TYPE_2D;
			view.format = swapchainFormat;
			view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			view.subresourceRange.levelCount = 1;
			view.subresourceRange.layerCount = 1;
			if (vkCreateImageView(device, &view, NULL, &imageViews[i]) != VK_SUCCESS)
				return false;
			VkSemaphoreCreateInfo semaphore = {};
			semaphore.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			if (vkCreateSemaphore(device, &semaphore, NULL, &presentSemaphores[i]) != VK_SUCCESS)
				return false;
		}
		resizePending = false;
		return createDepthResources();
	}

	bool createDepthResources()
	{
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, depthFormat, &formatProperties);
		if (!(formatProperties.optimalTilingFeatures &
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT))
			return false;
		depthImages.resize(images.size(), VK_NULL_HANDLE);
		depthMemories.resize(images.size(), VK_NULL_HANDLE);
		depthViews.resize(images.size(), VK_NULL_HANDLE);
		depthInitialized.resize(images.size(), false);
		for (size_t i = 0; i < images.size(); ++i)
		{
			VkImageCreateInfo imageInfo = {};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.format = depthFormat;
			imageInfo.extent.width = swapchainExtent.width;
			imageInfo.extent.height = swapchainExtent.height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			if (vkCreateImage(device, &imageInfo, NULL, &depthImages[i]) != VK_SUCCESS)
				return false;
			VkMemoryRequirements requirements;
			vkGetImageMemoryRequirements(device, depthImages[i], &requirements);
			const uint32 memoryType = findMemoryType(requirements.memoryTypeBits,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			if (memoryType == UINT32_MAX)
				return false;
			VkMemoryAllocateInfo allocation = {};
			allocation.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocation.allocationSize = requirements.size;
			allocation.memoryTypeIndex = memoryType;
			if (vkAllocateMemory(device, &allocation, NULL, &depthMemories[i]) != VK_SUCCESS ||
				vkBindImageMemory(device, depthImages[i], depthMemories[i], 0) != VK_SUCCESS)
				return false;
			VkImageViewCreateInfo viewInfo = {};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = depthImages[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = depthFormat;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.layerCount = 1;
			if (vkCreateImageView(device, &viewInfo, NULL, &depthViews[i]) != VK_SUCCESS)
				return false;
		}
		return true;
	}

	bool loadShaderBinary(const char* filename, std::vector<uint32>& words)
	{
		char executablePath[MAX_PATH] = {};
		DWORD pathLength = GetModuleFileNameA(NULL, executablePath, MAX_PATH);
		if (!pathLength || pathLength >= MAX_PATH)
			return false;
		char* slash = strrchr(executablePath, '\\');
		if (!slash)
			return false;
		*(slash + 1) = '\0';
		strcat_s(executablePath, filename);

		FILE* file = NULL;
		if (fopen_s(&file, executablePath, "rb") != 0 || !file)
		{
			fprintf(stderr, "gkVulkan: shader not found: %s\n", executablePath);
			return false;
		}
		fseek(file, 0, SEEK_END);
		long byteSize = ftell(file);
		fseek(file, 0, SEEK_SET);
		if (byteSize <= 0 || (byteSize % 4) != 0)
		{
			fclose(file);
			return false;
		}
		words.resize(static_cast<size_t>(byteSize) / sizeof(uint32));
		const size_t readCount = fread(&words[0], 1, static_cast<size_t>(byteSize), file);
		fclose(file);
		return readCount == static_cast<size_t>(byteSize);
	}

	bool createShaderModule(const std::vector<uint32>& words, VkShaderModule& shaderModule)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = words.size() * sizeof(uint32);
		createInfo.pCode = words.empty() ? NULL : &words[0];
		return vkCreateShaderModule(device, &createInfo, NULL, &shaderModule) == VK_SUCCESS;
	}

	bool createAcceptancePipeline()
	{
		std::vector<uint32> vertexWords;
		std::vector<uint32> fragmentWords;
		if (!loadShaderBinary("gk_vk_acceptance.vert.spv", vertexWords) ||
			!loadShaderBinary("gk_vk_acceptance.frag.spv", fragmentWords))
			return false;

		VkShaderModule vertexModule = VK_NULL_HANDLE;
		VkShaderModule fragmentModule = VK_NULL_HANDLE;
		if (!createShaderModule(vertexWords, vertexModule) ||
			!createShaderModule(fragmentWords, fragmentModule))
		{
			if (vertexModule)
				vkDestroyShaderModule(device, vertexModule, NULL);
			if (fragmentModule)
				vkDestroyShaderModule(device, fragmentModule, NULL);
			return false;
		}

		VkPushConstantRange pushConstants = {};
		pushConstants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstants.offset = 0;
		pushConstants.size = sizeof(float) * 4;
		VkPipelineLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.pushConstantRangeCount = 1;
		layoutInfo.pPushConstantRanges = &pushConstants;
		if (vkCreatePipelineLayout(device, &layoutInfo, NULL, &acceptancePipelineLayout) != VK_SUCCESS)
		{
			vkDestroyShaderModule(device, fragmentModule, NULL);
			vkDestroyShaderModule(device, vertexModule, NULL);
			return false;
		}

		VkPipelineShaderStageCreateInfo stages[2] = {};
		stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		stages[0].module = vertexModule;
		stages[0].pName = "main";
		stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		stages[1].module = fragmentModule;
		stages[1].pName = "main";

		VkPipelineVertexInputStateCreateInfo vertexInput = {};
		vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;
		VkPipelineRasterizationStateCreateInfo rasterization = {};
		rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterization.polygonMode = VK_POLYGON_MODE_FILL;
		rasterization.cullMode = VK_CULL_MODE_NONE;
		rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterization.lineWidth = 1.0f;
		VkPipelineMultisampleStateCreateInfo multisample = {};
		multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		VkPipelineDepthStencilStateCreateInfo depthStencil = {};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		VkPipelineColorBlendAttachmentState blendAttachment = {};
		blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		VkPipelineColorBlendStateCreateInfo colorBlend = {};
		colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlend.attachmentCount = 1;
		colorBlend.pAttachments = &blendAttachment;
		VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = 2;
		dynamicState.pDynamicStates = dynamicStates;
		VkPipelineRenderingCreateInfo renderingInfo = {};
		renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachmentFormats = &swapchainFormat;
		renderingInfo.depthAttachmentFormat = depthFormat;

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.pNext = &renderingInfo;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = stages;
		pipelineInfo.pVertexInputState = &vertexInput;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterization;
		pipelineInfo.pMultisampleState = &multisample;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlend;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = acceptancePipelineLayout;
		VkResult result = vkCreateGraphicsPipelines(
			device, pipelineCache, 1, &pipelineInfo, NULL, &acceptancePipeline);
		vkDestroyShaderModule(device, fragmentModule, NULL);
		vkDestroyShaderModule(device, vertexModule, NULL);
		if (result != VK_SUCCESS)
		{
			vkDestroyPipelineLayout(device, acceptancePipelineLayout, NULL);
			acceptancePipelineLayout = VK_NULL_HANDLE;
			return false;
		}
		return true;
	}

	void destroyAcceptancePipeline()
	{
		if (acceptancePipeline)
			vkDestroyPipeline(device, acceptancePipeline, NULL);
		acceptancePipeline = VK_NULL_HANDLE;
		if (acceptancePipelineLayout)
			vkDestroyPipelineLayout(device, acceptancePipelineLayout, NULL);
		acceptancePipelineLayout = VK_NULL_HANDLE;
	}

	bool createSceneDescriptorResources()
	{
		if (sceneDescriptorSetLayout)
			return true;
		VkDescriptorSetLayoutBinding textureBindings[kTextureStageCount] = {};
		for (uint32 stage = 0; stage < kTextureStageCount; ++stage)
		{
			textureBindings[stage].binding = stage;
			textureBindings[stage].descriptorType =
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			textureBindings[stage].descriptorCount = 1;
			textureBindings[stage].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		}
		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = kTextureStageCount;
		layoutInfo.pBindings = textureBindings;
		if (vkCreateDescriptorSetLayout(device, &layoutInfo, NULL,
			&sceneDescriptorSetLayout) != VK_SUCCESS)
			return false;
		VkDescriptorPoolSize poolSize = {};
		poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSize.descriptorCount = 4096 * kTextureStageCount;
		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.maxSets = 4096;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		if (vkCreateDescriptorPool(device, &poolInfo, NULL, &sceneDescriptorPool) != VK_SUCCESS)
			return false;
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = samplerAnisotropySupported ? VK_TRUE : VK_FALSE;
		samplerInfo.maxAnisotropy = samplerAnisotropySupported ?
			(std::min)(8.0f, maxSamplerAnisotropy) : 1.0f;
		samplerInfo.maxLod = 0.0f;
		return vkCreateSampler(device, &samplerInfo, NULL, &sceneSampler) == VK_SUCCESS;
	}

	bool createScenePipelines()
	{
		if (!createSceneDescriptorResources())
			return false;
		std::vector<uint32> vertexWords;
		std::vector<uint32> fragmentWords;
		std::vector<uint32> litVertexWords;
		std::vector<uint32> litFragmentWords;
		if (!loadShaderBinary("gk_vk_basic.vert.spv", vertexWords) ||
			!loadShaderBinary("gk_vk_basic.frag.spv", fragmentWords) ||
			!loadShaderBinary("gk_vk_lit.vert.spv", litVertexWords) ||
			!loadShaderBinary("gk_vk_lit.frag.spv", litFragmentWords))
			return false;

		VkShaderModule vertexModule = VK_NULL_HANDLE;
		VkShaderModule fragmentModule = VK_NULL_HANDLE;
		VkShaderModule litVertexModule = VK_NULL_HANDLE;
		VkShaderModule litFragmentModule = VK_NULL_HANDLE;
		if (!createShaderModule(vertexWords, vertexModule) ||
			!createShaderModule(fragmentWords, fragmentModule) ||
			!createShaderModule(litVertexWords, litVertexModule) ||
			!createShaderModule(litFragmentWords, litFragmentModule))
		{
			if (vertexModule)
				vkDestroyShaderModule(device, vertexModule, NULL);
			if (fragmentModule)
				vkDestroyShaderModule(device, fragmentModule, NULL);
			if (litVertexModule)
				vkDestroyShaderModule(device, litVertexModule, NULL);
			if (litFragmentModule)
				vkDestroyShaderModule(device, litFragmentModule, NULL);
			return false;
		}

		VkPushConstantRange pushConstants = {};
		pushConstants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstants.size = sizeof(Matrix44) + sizeof(ColorF) + sizeof(Vec4) + sizeof(Vec4);
		VkPipelineLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.setLayoutCount = 1;
		layoutInfo.pSetLayouts = &sceneDescriptorSetLayout;
		layoutInfo.pushConstantRangeCount = 1;
		layoutInfo.pPushConstantRanges = &pushConstants;
		if (vkCreatePipelineLayout(device, &layoutInfo, NULL, &scenePipelineLayout) != VK_SUCCESS)
		{
			vkDestroyShaderModule(device, fragmentModule, NULL);
			vkDestroyShaderModule(device, vertexModule, NULL);
			vkDestroyShaderModule(device, litFragmentModule, NULL);
			vkDestroyShaderModule(device, litVertexModule, NULL);
			return false;
		}

		VkPipelineShaderStageCreateInfo stages[2] = {};
		stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		stages[0].module = vertexModule;
		stages[0].pName = "main";
		stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		stages[1].module = fragmentModule;
		stages[1].pName = "main";

		const uint32 strides[5] = {
			sizeof(GKVL_GpuParticle), sizeof(GKVL_P3F2F4F3),
			sizeof(GKVL_P3F2F4F3F4U4), sizeof(GKVL_P3T2U4),
			sizeof(GKVL_Pt2T2T2T2T2)
		};
		for (uint32 pipelineIndex = 0; pipelineIndex < 5; ++pipelineIndex)
		{
			VkVertexInputBindingDescription binding = {};
			binding.binding = 0;
			binding.stride = strides[pipelineIndex];
			binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			const bool litLayout = pipelineIndex == eVI_P4F4F4 ||
				pipelineIndex == eVI_P4F4F4F4U4;
			stages[0].module = litLayout ? litVertexModule : vertexModule;
			stages[1].module = litLayout ? litFragmentModule : fragmentModule;
			VkVertexInputAttributeDescription attributes[4] = {};
			attributes[0].location = 0;
			attributes[0].binding = 0;
			attributes[0].format = (pipelineIndex == eVI_T2T2 ||
				pipelineIndex == eVI_PT2T2T2T2T2) ?
				VK_FORMAT_R32G32_SFLOAT : VK_FORMAT_R32G32B32_SFLOAT;
			attributes[1].location = 1;
			attributes[1].binding = 0;
			attributes[1].format = VK_FORMAT_R32G32_SFLOAT;
			attributes[1].offset = pipelineIndex == eVI_T2T2 ? 0 :
				(pipelineIndex == eVI_PT2T2T2T2T2 ? sizeof(Vec2) : sizeof(Vec3));
			if (litLayout)
			{
				attributes[2].location = 2;
				attributes[2].binding = 0;
				attributes[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
				attributes[2].offset = pipelineIndex == eVI_P4F4F4 ?
					sizeof(float) * 7 : sizeof(float) * 5;
				attributes[3].location = 3;
				attributes[3].binding = 0;
				attributes[3].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributes[3].offset = pipelineIndex == eVI_P4F4F4 ?
					sizeof(float) * 11 : sizeof(float) * 9;
			}
			else if (pipelineIndex == eVI_P3T2U4)
			{
				attributes[2].location = 2;
				attributes[2].binding = 0;
				// ColorB::pack_argb8888 is stored as B,G,R,A on little-endian hosts.
				attributes[2].format = VK_FORMAT_B8G8R8A8_UNORM;
				attributes[2].offset = sizeof(Vec3) + sizeof(Vec2);
			}
			else if (!litLayout)
			{
				// basic.vert always declares location 2. These layouts do not use
				// vertex color, so bind a harmless in-stride value for validation.
				attributes[2].location = 2;
				attributes[2].binding = 0;
				attributes[2].format = VK_FORMAT_R8G8B8A8_UNORM;
				attributes[2].offset = 0;
			}
			VkPipelineVertexInputStateCreateInfo vertexInput = {};
			vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInput.vertexBindingDescriptionCount = 1;
			vertexInput.pVertexBindingDescriptions = &binding;
			vertexInput.vertexAttributeDescriptionCount = litLayout ? 4 : 3;
			vertexInput.pVertexAttributeDescriptions = attributes;
			VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
			inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			VkPipelineViewportStateCreateInfo viewportState = {};
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.viewportCount = 1;
			viewportState.scissorCount = 1;
			VkPipelineRasterizationStateCreateInfo rasterization = {};
			rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterization.polygonMode = VK_POLYGON_MODE_FILL;
			rasterization.cullMode = VK_CULL_MODE_NONE;
			// gkEngine's exported meshes present their visible side as
			// counter-clockwise after the Vulkan clip-space conversion.
			rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
			rasterization.lineWidth = 1.0f;
			VkPipelineMultisampleStateCreateInfo multisample = {};
			multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			VkPipelineDepthStencilStateCreateInfo depthStencil = {};
			depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencil.depthTestEnable = VK_TRUE;
			depthStencil.depthWriteEnable = VK_TRUE;
			depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
			VkPipelineColorBlendAttachmentState blendAttachment = {};
			blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
				VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			VkPipelineColorBlendStateCreateInfo colorBlend = {};
			colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlend.attachmentCount = 1;
			colorBlend.pAttachments = &blendAttachment;
			VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
			VkPipelineDynamicStateCreateInfo dynamicState = {};
			dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicState.dynamicStateCount = 2;
			dynamicState.pDynamicStates = dynamicStates;
			VkPipelineRenderingCreateInfo renderingInfo = {};
			renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
			renderingInfo.colorAttachmentCount = 1;
			renderingInfo.pColorAttachmentFormats = &swapchainFormat;
			renderingInfo.depthAttachmentFormat = depthFormat;
			VkGraphicsPipelineCreateInfo pipelineInfo = {};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.pNext = &renderingInfo;
			pipelineInfo.stageCount = 2;
			pipelineInfo.pStages = stages;
			pipelineInfo.pVertexInputState = &vertexInput;
			pipelineInfo.pInputAssemblyState = &inputAssembly;
			pipelineInfo.pViewportState = &viewportState;
			pipelineInfo.pRasterizationState = &rasterization;
			pipelineInfo.pMultisampleState = &multisample;
			pipelineInfo.pDepthStencilState = &depthStencil;
			pipelineInfo.pColorBlendState = &colorBlend;
			pipelineInfo.pDynamicState = &dynamicState;
			pipelineInfo.layout = scenePipelineLayout;
			const VkBool32 alphaTestEnabled = VK_TRUE;
			VkSpecializationMapEntry alphaTestEntry = {};
			alphaTestEntry.constantID = 0;
			alphaTestEntry.size = sizeof(alphaTestEnabled);
			VkSpecializationInfo specialization = {};
			specialization.mapEntryCount = 1;
			specialization.pMapEntries = &alphaTestEntry;
			specialization.dataSize = sizeof(alphaTestEnabled);
			specialization.pData = &alphaTestEnabled;
			const VkBool32 alphaOnlyEnabled = VK_TRUE;
			VkSpecializationMapEntry alphaOnlyEntry = {};
			alphaOnlyEntry.constantID = 1;
			alphaOnlyEntry.size = sizeof(alphaOnlyEnabled);
			VkSpecializationInfo alphaOnlySpecialization = {};
			alphaOnlySpecialization.mapEntryCount = 1;
			alphaOnlySpecialization.pMapEntries = &alphaOnlyEntry;
			alphaOnlySpecialization.dataSize = sizeof(alphaOnlyEnabled);
			alphaOnlySpecialization.pData = &alphaOnlyEnabled;
			const VkBool32 vertexColorEnabled = VK_TRUE;
			VkSpecializationMapEntry vertexColorEntry = {};
			vertexColorEntry.constantID = 2;
			vertexColorEntry.size = sizeof(vertexColorEnabled);
			VkSpecializationInfo vertexColorSpecialization = {};
			vertexColorSpecialization.mapEntryCount = 1;
			vertexColorSpecialization.pMapEntries = &vertexColorEntry;
			vertexColorSpecialization.dataSize = sizeof(vertexColorEnabled);
			vertexColorSpecialization.pData = &vertexColorEnabled;
			for (uint32 sidedness = 0; sidedness < 2; ++sidedness)
			{
				rasterization.cullMode =
					sidedness ? VK_CULL_MODE_NONE : VK_CULL_MODE_BACK_BIT;
				depthStencil.depthWriteEnable = VK_TRUE;
				blendAttachment.blendEnable = VK_FALSE;
				stages[0].pSpecializationInfo = NULL;
				stages[1].pSpecializationInfo = NULL;
				if (vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineInfo,
					NULL, &scenePipelines[pipelineIndex][0][sidedness]) != VK_SUCCESS)
				{
					vkDestroyShaderModule(device, fragmentModule, NULL);
					vkDestroyShaderModule(device, vertexModule, NULL);
					vkDestroyShaderModule(device, litFragmentModule, NULL);
					vkDestroyShaderModule(device, litVertexModule, NULL);
					return false;
				}
				depthStencil.depthWriteEnable = VK_FALSE;
				blendAttachment.blendEnable = VK_TRUE;
				blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
				blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
				blendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
				blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
				blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
				blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
				if (vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineInfo,
					NULL, &scenePipelines[pipelineIndex][1][sidedness]) != VK_SUCCESS)
				{
					vkDestroyShaderModule(device, fragmentModule, NULL);
					vkDestroyShaderModule(device, vertexModule, NULL);
					vkDestroyShaderModule(device, litFragmentModule, NULL);
					vkDestroyShaderModule(device, litVertexModule, NULL);
					return false;
				}
				depthStencil.depthWriteEnable = VK_TRUE;
				blendAttachment.blendEnable = VK_FALSE;
				stages[1].pSpecializationInfo = &specialization;
				if (vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineInfo,
					NULL, &scenePipelines[pipelineIndex][2][sidedness]) != VK_SUCCESS)
				{
					vkDestroyShaderModule(device, fragmentModule, NULL);
					vkDestroyShaderModule(device, vertexModule, NULL);
					vkDestroyShaderModule(device, litFragmentModule, NULL);
					vkDestroyShaderModule(device, litVertexModule, NULL);
					return false;
				}
				depthStencil.depthTestEnable = VK_FALSE;
				depthStencil.depthWriteEnable = VK_FALSE;
				blendAttachment.blendEnable = VK_TRUE;
				stages[0].pSpecializationInfo =
					pipelineIndex == eVI_P3T2U4 ? &vertexColorSpecialization : NULL;
				stages[1].pSpecializationInfo = &alphaOnlySpecialization;
				if (vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineInfo,
					NULL, &scenePipelines[pipelineIndex][3][sidedness]) != VK_SUCCESS)
				{
					vkDestroyShaderModule(device, fragmentModule, NULL);
					vkDestroyShaderModule(device, vertexModule, NULL);
					vkDestroyShaderModule(device, litFragmentModule, NULL);
					vkDestroyShaderModule(device, litVertexModule, NULL);
					return false;
				}
				stages[0].pSpecializationInfo = NULL;
				stages[1].pSpecializationInfo = NULL;
				if (vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineInfo,
					NULL, &scenePipelines[pipelineIndex][4][sidedness]) != VK_SUCCESS)
				{
					vkDestroyShaderModule(device, fragmentModule, NULL);
					vkDestroyShaderModule(device, vertexModule, NULL);
					vkDestroyShaderModule(device, litFragmentModule, NULL);
					vkDestroyShaderModule(device, litVertexModule, NULL);
					return false;
				}
				depthStencil.depthTestEnable = VK_TRUE;
			}
			stages[0].pSpecializationInfo = NULL;
			stages[1].pSpecializationInfo = NULL;
		}
		vkDestroyShaderModule(device, fragmentModule, NULL);
		vkDestroyShaderModule(device, vertexModule, NULL);
		vkDestroyShaderModule(device, litFragmentModule, NULL);
		vkDestroyShaderModule(device, litVertexModule, NULL);
		return true;
	}

	void destroyScenePipelines()
	{
		for (uint32 i = 0; i < 5; ++i)
		{
			for (uint32 variant = 0; variant < 5; ++variant)
			{
				for (uint32 sidedness = 0; sidedness < 2; ++sidedness)
				{
					if (scenePipelines[i][variant][sidedness])
						vkDestroyPipeline(device,
							scenePipelines[i][variant][sidedness], NULL);
					scenePipelines[i][variant][sidedness] = VK_NULL_HANDLE;
				}
			}
		}
		if (scenePipelineLayout)
			vkDestroyPipelineLayout(device, scenePipelineLayout, NULL);
		scenePipelineLayout = VK_NULL_HANDLE;
	}

	void destroySceneDescriptorResources()
	{
		for (std::unordered_map<const ITexture*, GpuTexture>::iterator it = gpuTextures.begin();
			it != gpuTextures.end(); ++it)
		{
			if (it->second.view)
				vkDestroyImageView(device, it->second.view, NULL);
			if (it->second.image)
				vkDestroyImage(device, it->second.image, NULL);
			if (it->second.memory)
				vkFreeMemory(device, it->second.memory, NULL);
		}
		gpuTextures.clear();
		textureDescriptorSets.clear();
		if (sceneSampler)
			vkDestroySampler(device, sceneSampler, NULL);
		sceneSampler = VK_NULL_HANDLE;
		if (sceneDescriptorPool)
			vkDestroyDescriptorPool(device, sceneDescriptorPool, NULL);
		sceneDescriptorPool = VK_NULL_HANDLE;
		if (sceneDescriptorSetLayout)
			vkDestroyDescriptorSetLayout(device, sceneDescriptorSetLayout, NULL);
		sceneDescriptorSetLayout = VK_NULL_HANDLE;
	}

	uint32 findMemoryType(uint32 typeBits, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
		for (uint32 i = 0; i < memoryProperties.memoryTypeCount; ++i)
			if ((typeBits & (1u << i)) &&
				(memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;
		return UINT32_MAX;
	}

	bool createTransientBuffer(const void* source, VkDeviceSize size,
		VkBufferUsageFlags usage, VkBuffer& buffer)
	{
		buffer = VK_NULL_HANDLE;
		if (!source || !size)
			return false;
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		if (vkCreateBuffer(device, &bufferInfo, NULL, &buffer) != VK_SUCCESS)
			return false;
		VkMemoryRequirements requirements;
		vkGetBufferMemoryRequirements(device, buffer, &requirements);
		const uint32 memoryType = findMemoryType(requirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		if (memoryType == UINT32_MAX)
		{
			vkDestroyBuffer(device, buffer, NULL);
			buffer = VK_NULL_HANDLE;
			return false;
		}
		VkMemoryAllocateInfo allocate = {};
		allocate.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocate.allocationSize = requirements.size;
		allocate.memoryTypeIndex = memoryType;
		VkDeviceMemory memory = VK_NULL_HANDLE;
		if (vkAllocateMemory(device, &allocate, NULL, &memory) != VK_SUCCESS ||
			vkBindBufferMemory(device, buffer, memory, 0) != VK_SUCCESS)
		{
			if (memory)
				vkFreeMemory(device, memory, NULL);
			vkDestroyBuffer(device, buffer, NULL);
			buffer = VK_NULL_HANDLE;
			return false;
		}
		void* mapped = NULL;
		if (vkMapMemory(device, memory, 0, size, 0, &mapped) != VK_SUCCESS)
		{
			vkFreeMemory(device, memory, NULL);
			vkDestroyBuffer(device, buffer, NULL);
			buffer = VK_NULL_HANDLE;
			return false;
		}
		memcpy(mapped, source, static_cast<size_t>(size));
		vkUnmapMemory(device, memory);
		FrameContext::TransientBuffer transient = { buffer, memory };
		frames[frameIndex].transientBuffers.push_back(transient);
		return true;
	}

	bool createPersistentBuffer(const void* source, VkDeviceSize size,
		VkBufferUsageFlags usage, GpuBuffer& result)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		if (vkCreateBuffer(device, &bufferInfo, NULL, &result.buffer) != VK_SUCCESS)
			return false;
		VkMemoryRequirements requirements;
		vkGetBufferMemoryRequirements(device, result.buffer, &requirements);
		const uint32 memoryType = findMemoryType(requirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		VkMemoryAllocateInfo allocation = {};
		allocation.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocation.allocationSize = requirements.size;
		allocation.memoryTypeIndex = memoryType;
		if (memoryType == UINT32_MAX ||
			vkAllocateMemory(device, &allocation, NULL, &result.memory) != VK_SUCCESS ||
			vkBindBufferMemory(device, result.buffer, result.memory, 0) != VK_SUCCESS)
		{
			if (result.memory)
				vkFreeMemory(device, result.memory, NULL);
			if (result.buffer)
				vkDestroyBuffer(device, result.buffer, NULL);
			result = GpuBuffer();
			return false;
		}
		void* mapped = NULL;
		if (vkMapMemory(device, result.memory, 0, size, 0, &mapped) != VK_SUCCESS)
		{
			vkFreeMemory(device, result.memory, NULL);
			vkDestroyBuffer(device, result.buffer, NULL);
			result = GpuBuffer();
			return false;
		}
		memcpy(mapped, source, static_cast<size_t>(size));
		vkUnmapMemory(device, result.memory);
		result.size = size;
		return true;
	}

	VkBuffer getVertexBuffer(gkVertexBuffer* source)
	{
		std::unordered_map<const gkVertexBuffer*, GpuBuffer>::iterator existing =
			gpuVertexBuffers.find(source);
		if (existing != gpuVertexBuffers.end())
			return existing->second.buffer;
		GpuBuffer buffer;
		if (!source || !source->data ||
			!createPersistentBuffer(source->data, source->getSize(),
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, buffer))
			return VK_NULL_HANDLE;
		return gpuVertexBuffers.insert(std::make_pair(source, buffer)).first->second.buffer;
	}

	VkBuffer getIndexBuffer(gkIndexBuffer* source)
	{
		std::unordered_map<const gkIndexBuffer*, GpuBuffer>::iterator existing =
			gpuIndexBuffers.find(source);
		if (existing != gpuIndexBuffers.end())
			return existing->second.buffer;
		GpuBuffer buffer;
		if (!source || !source->data ||
			!createPersistentBuffer(source->data, source->getSize(),
				VK_BUFFER_USAGE_INDEX_BUFFER_BIT, buffer))
			return VK_NULL_HANDLE;
		return gpuIndexBuffers.insert(std::make_pair(source, buffer)).first->second.buffer;
	}

	void destroyPersistentBuffers()
	{
		for (std::unordered_map<const gkVertexBuffer*, GpuBuffer>::iterator it =
			gpuVertexBuffers.begin(); it != gpuVertexBuffers.end(); ++it)
		{
			vkDestroyBuffer(device, it->second.buffer, NULL);
			vkFreeMemory(device, it->second.memory, NULL);
		}
		gpuVertexBuffers.clear();
		for (std::unordered_map<const gkIndexBuffer*, GpuBuffer>::iterator it =
			gpuIndexBuffers.begin(); it != gpuIndexBuffers.end(); ++it)
		{
			vkDestroyBuffer(device, it->second.buffer, NULL);
			vkFreeMemory(device, it->second.memory, NULL);
		}
		gpuIndexBuffers.clear();
	}

	void destroyTransientBuffers(FrameContext& frame)
	{
		for (size_t i = 0; i < frame.transientBuffers.size(); ++i)
		{
			vkDestroyBuffer(device, frame.transientBuffers[i].buffer, NULL);
			vkFreeMemory(device, frame.transientBuffers[i].memory, NULL);
		}
		frame.transientBuffers.clear();
	}

	void destroyGpuTexture(GpuTexture& texture)
	{
		if (texture.view)
			vkDestroyImageView(device, texture.view, NULL);
		if (texture.image)
			vkDestroyImage(device, texture.image, NULL);
		if (texture.memory)
			vkFreeMemory(device, texture.memory, NULL);
		texture = GpuTexture();
	}

	bool uploadGpuTexture(GpuTexture& texture, const void* source,
		VkDeviceSize dataSize, bool initialized)
	{
		VkBuffer staging = VK_NULL_HANDLE;
		if (!source || !dataSize || !createTransientBuffer(source, dataSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, staging))
			return false;
		FrameContext& frame = frames[frameIndex];
		VkImageMemoryBarrier2 toTransfer = {};
		toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		toTransfer.srcStageMask = initialized ?
			VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT : VK_PIPELINE_STAGE_2_NONE;
		toTransfer.srcAccessMask = initialized ?
			VK_ACCESS_2_SHADER_SAMPLED_READ_BIT : VK_ACCESS_2_NONE;
		toTransfer.dstStageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
		toTransfer.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
		toTransfer.oldLayout = initialized ?
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED;
		toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		toTransfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toTransfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toTransfer.image = texture.image;
		toTransfer.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		toTransfer.subresourceRange.levelCount = 1;
		toTransfer.subresourceRange.layerCount = 1;
		VkDependencyInfo dependency = {};
		dependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		dependency.imageMemoryBarrierCount = 1;
		dependency.pImageMemoryBarriers = &toTransfer;
		cmdPipelineBarrier2(frame.commandBuffer, &dependency);
		VkBufferImageCopy copy = {};
		copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copy.imageSubresource.layerCount = 1;
		copy.imageExtent.width = texture.width;
		copy.imageExtent.height = texture.height;
		copy.imageExtent.depth = 1;
		vkCmdCopyBufferToImage(frame.commandBuffer, staging, texture.image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
		VkImageMemoryBarrier2 toShader = toTransfer;
		toShader.srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
		toShader.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
		toShader.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		toShader.dstAccessMask = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
		toShader.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		toShader.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		dependency.pImageMemoryBarriers = &toShader;
		cmdPipelineBarrier2(frame.commandBuffer, &dependency);
		return true;
	}

	GpuTexture* getOrCreateTexture(ITexture* texture)
	{
		std::unordered_map<const ITexture*, GpuTexture>::iterator existing =
			gpuTextures.find(texture);
		if (existing != gpuTextures.end())
		{
			ITexture* dynamicTexture =
				texture && texture->dynamic() ? texture : NULL;
			if (dynamicTexture && !renderingActive &&
				existing->second.revision != dynamicTexture->revision() &&
				dynamicTexture->RawData())
			{
				const bool singleChannel =
					dynamicTexture->format() == eTF_A8 ||
					dynamicTexture->format() == eTF_R8;
				const VkDeviceSize dataSize =
					static_cast<VkDeviceSize>(dynamicTexture->getWidth()) *
					dynamicTexture->getHeight() * (singleChannel ? 1 : 4);
				if (uploadGpuTexture(existing->second,
					dynamicTexture->RawData(), dataSize, true))
					existing->second.revision = dynamicTexture->revision();
			}
			return &existing->second;
		}
		if (renderingActive)
			return NULL;

		const uint8 whitePixel[4] = { 255, 255, 255, 255 };
		const void* source = whitePixel;
		uint32 width = 1;
		uint32 height = 1;
		VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
		VkDeviceSize dataSize = sizeof(whitePixel);
		if (texture)
		{
			texture->touch();
			if (texture->RawData() && texture->getWidth() && texture->getHeight())
			{
				source = texture->RawData();
				width = texture->getWidth();
				height = texture->getHeight();
				const bool singleChannel =
					texture->format() == eTF_A8 || texture->format() == eTF_R8;
				format = singleChannel ? VK_FORMAT_R8_UNORM : VK_FORMAT_R8G8B8A8_UNORM;
				dataSize = static_cast<VkDeviceSize>(width) * height *
					(singleChannel ? 1 : 4);
			}
		}

		GpuTexture gpuTexture;
		gpuTexture.width = width;
		gpuTexture.height = height;
		gpuTexture.revision = texture ? texture->revision() : 0;
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.format = format;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		if (vkCreateImage(device, &imageInfo, NULL, &gpuTexture.image) != VK_SUCCESS)
			return NULL;
		VkMemoryRequirements requirements;
		vkGetImageMemoryRequirements(device, gpuTexture.image, &requirements);
		const uint32 memoryType = findMemoryType(requirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VkMemoryAllocateInfo allocation = {};
		allocation.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocation.allocationSize = requirements.size;
		allocation.memoryTypeIndex = memoryType;
		if (memoryType == UINT32_MAX ||
			vkAllocateMemory(device, &allocation, NULL, &gpuTexture.memory) != VK_SUCCESS ||
			vkBindImageMemory(device, gpuTexture.image, gpuTexture.memory, 0) != VK_SUCCESS)
		{
			destroyGpuTexture(gpuTexture);
			return NULL;
		}
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = gpuTexture.image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.layerCount = 1;
		if (vkCreateImageView(device, &viewInfo, NULL, &gpuTexture.view) != VK_SUCCESS)
		{
			destroyGpuTexture(gpuTexture);
			return NULL;
		}

		if (!uploadGpuTexture(gpuTexture, source, dataSize, false))
		{
			destroyGpuTexture(gpuTexture);
			return NULL;
		}

		std::pair<std::unordered_map<const ITexture*, GpuTexture>::iterator, bool> inserted =
			gpuTextures.insert(std::make_pair(texture, gpuTexture));
		return &inserted.first->second;
	}

	VkDescriptorSet getOrCreateTextureSet(ITexture* const* textures, uint32 textureCount)
	{
		if (renderingActive)
			return VK_NULL_HANDLE;
		std::vector<const ITexture*> key(kTextureStageCount, NULL);
		std::vector<GpuTexture*> gpu(kTextureStageCount, NULL);
		for (uint32 stage = 0; stage < kTextureStageCount; ++stage)
		{
			ITexture* texture = textures && stage < textureCount ?
				textures[stage] : NULL;
			key[stage] = texture;
			gpu[stage] = getOrCreateTexture(texture);
			if (!gpu[stage])
				return VK_NULL_HANDLE;
		}
		std::map<std::vector<const ITexture*>, VkDescriptorSet>::iterator existing =
			textureDescriptorSets.find(key);
		if (existing != textureDescriptorSets.end())
			return existing->second;
		VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
		VkDescriptorSetAllocateInfo allocation = {};
		allocation.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocation.descriptorPool = sceneDescriptorPool;
		allocation.descriptorSetCount = 1;
		allocation.pSetLayouts = &sceneDescriptorSetLayout;
		if (vkAllocateDescriptorSets(device, &allocation, &descriptorSet) != VK_SUCCESS)
			return VK_NULL_HANDLE;
		VkDescriptorImageInfo images[kTextureStageCount] = {};
		VkWriteDescriptorSet writes[kTextureStageCount] = {};
		for (uint32 stage = 0; stage < kTextureStageCount; ++stage)
		{
			images[stage].sampler = sceneSampler;
			images[stage].imageView = gpu[stage]->view;
			images[stage].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			writes[stage].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writes[stage].dstSet = descriptorSet;
			writes[stage].dstBinding = stage;
			writes[stage].descriptorCount = 1;
			writes[stage].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writes[stage].pImageInfo = &images[stage];
		}
		vkUpdateDescriptorSets(device, kTextureStageCount, writes, 0, NULL);
		textureDescriptorSets.insert(std::make_pair(key, descriptorSet));
		return descriptorSet;
	}

	void destroySwapchain()
	{
		for (size_t i = 0; i < depthViews.size(); ++i)
			if (depthViews[i])
				vkDestroyImageView(device, depthViews[i], NULL);
		for (size_t i = 0; i < depthImages.size(); ++i)
			if (depthImages[i])
				vkDestroyImage(device, depthImages[i], NULL);
		for (size_t i = 0; i < depthMemories.size(); ++i)
			if (depthMemories[i])
				vkFreeMemory(device, depthMemories[i], NULL);
		depthViews.clear();
		depthImages.clear();
		depthMemories.clear();
		depthInitialized.clear();
		for (size_t i = 0; i < presentSemaphores.size(); ++i)
			if (presentSemaphores[i])
				vkDestroySemaphore(device, presentSemaphores[i], NULL);
		presentSemaphores.clear();
		for (size_t i = 0; i < imageViews.size(); ++i)
			if (imageViews[i])
				vkDestroyImageView(device, imageViews[i], NULL);
		imageViews.clear();
		images.clear();
		imageInitialized.clear();
		if (swapchain)
			vkDestroySwapchainKHR(device, swapchain, NULL);
		swapchain = VK_NULL_HANDLE;
		swapchainExtent.width = 0;
		swapchainExtent.height = 0;
	}

	bool recreateSwapchain()
	{
		if (isMinimized || requestedWidth == 0 || requestedHeight == 0)
			return false;
		VkFence fences[kFramesInFlight];
		for (uint32 i = 0; i < kFramesInFlight; ++i)
			fences[i] = frames[i].fence;
		vkWaitForFences(device, kFramesInFlight, fences, VK_TRUE, UINT64_MAX);
		vkQueueWaitIdle(presentQueue);
		destroyScenePipelines();
		destroyAcceptancePipeline();
		destroySwapchain();
		return createSwapchain() && createAcceptancePipeline() && createScenePipelines();
	}

	bool createFrames()
	{
		for (uint32 i = 0; i < kFramesInFlight; ++i)
		{
			VkCommandPoolCreateInfo pool = {};
			pool.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			pool.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			pool.queueFamilyIndex = graphicsFamily;
			if (vkCreateCommandPool(device, &pool, NULL, &frames[i].commandPool) != VK_SUCCESS)
				return false;

			VkCommandBufferAllocateInfo allocate = {};
			allocate.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocate.commandPool = frames[i].commandPool;
			allocate.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocate.commandBufferCount = 1;
			if (vkAllocateCommandBuffers(device, &allocate, &frames[i].commandBuffer) != VK_SUCCESS)
				return false;

			VkSemaphoreCreateInfo semaphore = {};
			semaphore.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			if (vkCreateSemaphore(device, &semaphore, NULL, &frames[i].imageAvailable) != VK_SUCCESS)
				return false;

			VkFenceCreateInfo fence = {};
			fence.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fence.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			if (vkCreateFence(device, &fence, NULL, &frames[i].fence) != VK_SUCCESS)
				return false;
			if (timestampsSupported)
			{
				VkQueryPoolCreateInfo queryInfo = {};
				queryInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
				queryInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
				queryInfo.queryCount = 2;
				if (vkCreateQueryPool(device, &queryInfo, NULL,
					&frames[i].timestampPool) != VK_SUCCESS)
				{
					timestampsSupported = false;
					frames[i].timestampPool = VK_NULL_HANDLE;
				}
			}
		}
		return true;
	}

	bool initialize(const ISystemInitInfo& initInfo)
	{
		if (!createWindow(initInfo))
			return failInitialization("Win32 window");
		if (!createInstance())
			return failInitialization("Vulkan instance");
		if (!createSurface())
			return failInitialization("Win32 surface");
		if (!selectPhysicalDevice())
			return failInitialization("physical device");
		if (!createDevice())
			return failInitialization("logical device");
		if (!createPipelineCache())
			return failInitialization("pipeline cache");
		if (!createSwapchain())
			return failInitialization("swapchain");
		if (!createAcceptancePipeline())
			return failInitialization("acceptance graphics pipeline");
		if (!createScenePipelines())
			return failInitialization("scene graphics pipelines");
		if (!createFrames())
			return failInitialization("frame contexts");
		return true;
	}

	bool failInitialization(const char* stage)
	{
		fprintf(stderr, "gkVulkan: failed to create %s\n", stage);
		shutdown();
		return false;
	}

	bool beginFrame()
	{
		frameActive = false;
		if (!device || isMinimized)
			return false;
		if (resizePending && !recreateSwapchain())
			return false;

		FrameContext& frame = frames[frameIndex];
		if (vkWaitForFences(device, 1, &frame.fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS)
			return false;
		if (timestampsSupported && frame.timestampPool && frame.timestampsIssued)
		{
			uint64 timestamps[2] = {};
			if (vkGetQueryPoolResults(device, frame.timestampPool, 0, 2,
				sizeof(timestamps), timestamps, sizeof(uint64),
				VK_QUERY_RESULT_64_BIT) == VK_SUCCESS)
			{
				uint64 delta = timestamps[1] - timestamps[0];
				if (timestampValidBits < 64)
					delta &= (1ull << timestampValidBits) - 1ull;
				lastGpuFrameMs = static_cast<float>(
					static_cast<double>(delta) * timestampPeriod / 1000000.0);
			}
		}
		destroyTransientBuffers(frame);
		VkResult acquire = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX,
			frame.imageAvailable, VK_NULL_HANDLE, &acquiredImage);
		if (acquire == VK_ERROR_OUT_OF_DATE_KHR)
		{
			resizePending = true;
			return false;
		}
		if (acquire != VK_SUCCESS && acquire != VK_SUBOPTIMAL_KHR)
			return false;
		if (acquire == VK_SUBOPTIMAL_KHR)
			resizePending = true;

		vkResetFences(device, 1, &frame.fence);
		vkResetCommandPool(device, frame.commandPool, 0);
		VkCommandBufferBeginInfo begin = {};
		begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		if (vkBeginCommandBuffer(frame.commandBuffer, &begin) != VK_SUCCESS)
			return false;
		if (timestampsSupported && frame.timestampPool)
		{
			vkCmdResetQueryPool(frame.commandBuffer, frame.timestampPool, 0, 2);
			vkCmdWriteTimestamp(frame.commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				frame.timestampPool, 0);
			frame.timestampsIssued = true;
		}

		VkImageMemoryBarrier2 toColor = {};
		toColor.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		toColor.srcStageMask = imageInitialized[acquiredImage] ?
			VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT : VK_PIPELINE_STAGE_2_NONE;
		toColor.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		toColor.srcAccessMask = VK_ACCESS_2_NONE;
		toColor.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
		toColor.oldLayout = imageInitialized[acquiredImage] ?
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_UNDEFINED;
		toColor.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		toColor.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toColor.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toColor.image = images[acquiredImage];
		toColor.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		toColor.subresourceRange.levelCount = 1;
		toColor.subresourceRange.layerCount = 1;
		VkDependencyInfo dependency = {};
		dependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		dependency.imageMemoryBarrierCount = 1;
		dependency.pImageMemoryBarriers = &toColor;
		cmdPipelineBarrier2(frame.commandBuffer, &dependency);
		VkImageMemoryBarrier2 toDepth = {};
		toDepth.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		toDepth.srcStageMask = depthInitialized[acquiredImage] ?
			VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT : VK_PIPELINE_STAGE_2_NONE;
		toDepth.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
		toDepth.srcAccessMask = depthInitialized[acquiredImage] ?
			VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT : VK_ACCESS_2_NONE;
		toDepth.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
			VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		toDepth.oldLayout = depthInitialized[acquiredImage] ?
			VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED;
		toDepth.newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
		toDepth.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toDepth.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toDepth.image = depthImages[acquiredImage];
		toDepth.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		toDepth.subresourceRange.levelCount = 1;
		toDepth.subresourceRange.layerCount = 1;
		dependency.pImageMemoryBarriers = &toDepth;
		cmdPipelineBarrier2(frame.commandBuffer, &dependency);

		frameDrawCount = 0;
		renderingActive = false;
		frameActive = true;
		return true;
	}

	bool beginSceneRendering()
	{
		if (!frameActive)
			return false;
		if (renderingActive)
			return true;
		FrameContext& frame = frames[frameIndex];
		VkClearValue clear = {};
		clear.color.float32[0] = 0.035f;
		clear.color.float32[1] = 0.055f;
		clear.color.float32[2] = 0.085f;
		clear.color.float32[3] = 1.0f;
		VkRenderingAttachmentInfo colorAttachment = {};
		colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachment.imageView = imageViews[acquiredImage];
		colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.clearValue = clear;
		VkClearValue depthClear = {};
		depthClear.depthStencil.depth = 1.0f;
		VkRenderingAttachmentInfo depthAttachment = {};
		depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		depthAttachment.imageView = depthViews[acquiredImage];
		depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.clearValue = depthClear;
		VkRenderingInfo rendering = {};
		rendering.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		rendering.renderArea.extent = swapchainExtent;
		rendering.layerCount = 1;
		rendering.colorAttachmentCount = 1;
		rendering.pColorAttachments = &colorAttachment;
		rendering.pDepthAttachment = &depthAttachment;
		cmdBeginRendering(frame.commandBuffer, &rendering);
		VkViewport viewport = {};
		viewport.width = static_cast<float>(swapchainExtent.width);
		viewport.height = static_cast<float>(swapchainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor = {};
		scissor.extent = swapchainExtent;
		vkCmdSetViewport(frame.commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(frame.commandBuffer, 0, 1, &scissor);
		renderingActive = true;
		return true;
	}

	bool prepareTexture(ITexture* texture)
	{
		if (!frameActive || renderingActive)
			return false;
		ITexture* textures[1] = { texture };
		return getOrCreateTextureSet(textures, 1) != VK_NULL_HANDLE;
	}

	bool prepareTextures(ITexture* const* textures, uint32 textureCount)
	{
		if (!frameActive || renderingActive)
			return false;
		return getOrCreateTextureSet(textures, textureCount) != VK_NULL_HANDLE;
	}

	bool drawRenderOperation(const gkRenderOperation& operation,
		const Matrix44& worldViewProjection, const ColorF& color,
		ITexture* const* textures, uint32 textureCount, const Vec2& uvTiling,
		bool transparent, bool alphaTest,
		const Vec3& lightDirection, bool doubleSided, bool alphaOnlyTexture,
		bool overlay)
	{
		if (!frameActive || operation.operationType != gkRenderOperation::OT_TRIANGLE_LIST ||
			!operation.vertexData || !operation.vertexData->data ||
			operation.vertexData->vertexType > eVI_PT2T2T2T2T2)
			return false;
		const uint32 vertexType = static_cast<uint32>(operation.vertexData->vertexType);
		VkBuffer vertexBuffer = VK_NULL_HANDLE;
		if (operation.vertexData->needRebind())
		{
			if (!createTransientBuffer(operation.vertexData->data,
				operation.vertexData->getSize(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBuffer))
				return false;
		}
		else
			vertexBuffer = getVertexBuffer(operation.vertexData);
		if (!vertexBuffer)
			return false;
		std::vector<const ITexture*> key(kTextureStageCount, NULL);
		for (uint32 stage = 0; stage < kTextureStageCount; ++stage)
			key[stage] = textures && stage < textureCount ? textures[stage] : NULL;
		std::map<std::vector<const ITexture*>, VkDescriptorSet>::iterator descriptor =
			textureDescriptorSets.find(key);
		if (descriptor == textureDescriptorSets.end() || !beginSceneRendering())
			return false;
		FrameContext& frame = frames[frameIndex];
		VkDeviceSize vertexOffset =
			static_cast<VkDeviceSize>(operation.vertexStart) * operation.vertexData->elementSize;
		vkCmdBindPipeline(frame.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			scenePipelines[vertexType][alphaOnlyTexture ? 3 :
				(overlay ? 4 : (transparent ? 1 : (alphaTest ? 2 : 0)))]
				[doubleSided ? 1 : 0]);
		vkCmdBindDescriptorSets(frame.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			scenePipelineLayout, 0, 1, &descriptor->second, 0, NULL);
		vkCmdBindVertexBuffers(frame.commandBuffer, 0, 1, &vertexBuffer, &vertexOffset);
		struct DrawConstants
		{
			Matrix44 worldViewProjection;
			ColorF color;
			Vec4 uvTransform;
			Vec4 lightDirection;
		} constants;
		constants.worldViewProjection = worldViewProjection;
		constants.color = color;
		constants.uvTransform = Vec4(uvTiling.x, uvTiling.y, 0.0f, 0.0f);
		constants.lightDirection = Vec4(
			lightDirection.x, lightDirection.y, lightDirection.z, 0.0f);
		vkCmdPushConstants(frame.commandBuffer, scenePipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(constants), &constants);

		if (operation.useIndexes && operation.indexData && operation.indexData->data)
		{
			VkBuffer indexBuffer = VK_NULL_HANDLE;
			if (operation.indexData->needRebind())
			{
				if (!createTransientBuffer(operation.indexData->data,
					operation.indexData->getSize(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBuffer))
					return false;
			}
			else
				indexBuffer = getIndexBuffer(operation.indexData);
			if (!indexBuffer)
				return false;
			const VkIndexType indexType = operation.indexData->wordbit ?
				VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;
			vkCmdBindIndexBuffer(frame.commandBuffer, indexBuffer, 0, indexType);
			vkCmdDrawIndexed(frame.commandBuffer, operation.indexCount, 1,
				operation.indexStart, 0, 0);
		}
		else
			vkCmdDraw(frame.commandBuffer, operation.vertexCount, 1, 0, 0);
		++frameDrawCount;
		return true;
	}

	bool endFrame()
	{
		if (!frameActive)
			return false;
		FrameContext& frame = frames[frameIndex];
		if (!beginSceneRendering())
			return false;
		if (frameDrawCount == 0)
		{
			struct DrawParams
			{
				float time;
				float aspect;
				float padding[2];
			} drawParams;
			drawParams.time = static_cast<float>(GetTickCount64() % 1000000) * 0.001f;
			drawParams.aspect = static_cast<float>(swapchainExtent.width) /
				static_cast<float>(swapchainExtent.height);
			drawParams.padding[0] = 0.0f;
			drawParams.padding[1] = 0.0f;
			vkCmdBindPipeline(frame.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				acceptancePipeline);
			vkCmdPushConstants(frame.commandBuffer, acceptancePipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(drawParams), &drawParams);
			vkCmdDraw(frame.commandBuffer, 3, 1, 0, 0);
		}
		cmdEndRendering(frame.commandBuffer);
		renderingActive = false;

		VkImageMemoryBarrier2 toPresent = {};
		toPresent.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		toPresent.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		toPresent.dstStageMask = VK_PIPELINE_STAGE_2_NONE;
		toPresent.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
		toPresent.dstAccessMask = VK_ACCESS_2_NONE;
		toPresent.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		toPresent.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		toPresent.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toPresent.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toPresent.image = images[acquiredImage];
		toPresent.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		toPresent.subresourceRange.levelCount = 1;
		toPresent.subresourceRange.layerCount = 1;
		VkDependencyInfo dependency = {};
		dependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		dependency.imageMemoryBarrierCount = 1;
		dependency.pImageMemoryBarriers = &toPresent;
		cmdPipelineBarrier2(frame.commandBuffer, &dependency);
		if (timestampsSupported && frame.timestampPool)
			vkCmdWriteTimestamp(frame.commandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				frame.timestampPool, 1);
		if (vkEndCommandBuffer(frame.commandBuffer) != VK_SUCCESS)
		{
			frameActive = false;
			return false;
		}
		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo submit = {};
		submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit.waitSemaphoreCount = 1;
		submit.pWaitSemaphores = &frame.imageAvailable;
		submit.pWaitDstStageMask = &waitStage;
		submit.commandBufferCount = 1;
		submit.pCommandBuffers = &frame.commandBuffer;
		submit.signalSemaphoreCount = 1;
		submit.pSignalSemaphores = &presentSemaphores[acquiredImage];
		if (vkQueueSubmit(graphicsQueue, 1, &submit, frame.fence) != VK_SUCCESS)
		{
			frameActive = false;
			return false;
		}

		VkPresentInfoKHR present = {};
		present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present.waitSemaphoreCount = 1;
		present.pWaitSemaphores = &presentSemaphores[acquiredImage];
		present.swapchainCount = 1;
		present.pSwapchains = &swapchain;
		present.pImageIndices = &acquiredImage;
		VkResult result = vkQueuePresentKHR(presentQueue, &present);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			resizePending = true;
		else if (result != VK_SUCCESS)
		{
			frameActive = false;
			return false;
		}
		imageInitialized[acquiredImage] = true;
		depthInitialized[acquiredImage] = true;
		frameIndex = (frameIndex + 1) % kFramesInFlight;
		frameActive = false;
		return true;
	}

	void shutdown()
	{
		frameActive = false;
		renderingActive = false;
		if (device)
			vkDeviceWaitIdle(device);
		if (device)
		{
			destroyScenePipelines();
			destroySceneDescriptorResources();
			destroyPersistentBuffers();
			destroyAcceptancePipeline();
			saveAndDestroyPipelineCache();
			for (uint32 i = 0; i < kFramesInFlight; ++i)
			{
				destroyTransientBuffers(frames[i]);
				if (frames[i].fence)
					vkDestroyFence(device, frames[i].fence, NULL);
				if (frames[i].imageAvailable)
					vkDestroySemaphore(device, frames[i].imageAvailable, NULL);
				if (frames[i].timestampPool)
					vkDestroyQueryPool(device, frames[i].timestampPool, NULL);
				if (frames[i].commandPool)
					vkDestroyCommandPool(device, frames[i].commandPool, NULL);
				frames[i] = FrameContext();
			}
			destroySwapchain();
			vkDestroyDevice(device, NULL);
			device = VK_NULL_HANDLE;
		}
		if (surface && instance)
			vkDestroySurfaceKHR(instance, surface, NULL);
		surface = VK_NULL_HANDLE;
		if (debugMessenger && destroyDebugMessenger)
			destroyDebugMessenger(instance, debugMessenger, NULL);
		debugMessenger = VK_NULL_HANDLE;
		if (instance)
			vkDestroyInstance(instance, NULL);
		instance = VK_NULL_HANDLE;
		if (ownsWindow && hwnd)
			DestroyWindow(hwnd);
		hwnd = NULL;
		ownsWindow = false;
		if (windowClassRegistered)
			UnregisterClass(kWindowClassName, hinstance);
		windowClassRegistered = false;
		physicalDevice = VK_NULL_HANDLE;
	}
};

gkVkDeviceContext::gkVkDeviceContext()
	: m_impl(new Impl())
{
}

gkVkDeviceContext::~gkVkDeviceContext()
{
	shutdown();
	delete m_impl;
}

HWND gkVkDeviceContext::initialize(const ISystemInitInfo& initInfo)
{
	if (m_impl->instance)
		return m_impl->hwnd;
	return m_impl->initialize(initInfo) ? m_impl->hwnd : NULL;
}

void gkVkDeviceContext::shutdown()
{
	m_impl->shutdown();
}

bool gkVkDeviceContext::beginFrame()
{
	return m_impl->beginFrame();
}

bool gkVkDeviceContext::endFrame()
{
	return m_impl->endFrame();
}

bool gkVkDeviceContext::prepareTexture(ITexture* texture)
{
	return m_impl->prepareTexture(texture);
}

bool gkVkDeviceContext::prepareTextures(ITexture* const* textures, uint32 textureCount)
{
	return m_impl->prepareTextures(textures, textureCount);
}

bool gkVkDeviceContext::drawRenderOperation(const gkRenderOperation& operation,
	const Matrix44& worldViewProjection, const ColorF& color,
	ITexture* texture, const Vec2& uvTiling, bool transparent, bool alphaTest,
	const Vec3& lightDirection, bool doubleSided, bool alphaOnlyTexture,
	bool overlay)
{
	ITexture* textures[1] = { texture };
	return m_impl->drawRenderOperation(operation, worldViewProjection, color,
		textures, 1, uvTiling, transparent, alphaTest, lightDirection, doubleSided,
		alphaOnlyTexture, overlay);
}

bool gkVkDeviceContext::drawRenderOperationTextures(
	const gkRenderOperation& operation, const Matrix44& worldViewProjection,
	const ColorF& color, ITexture* const* textures, uint32 textureCount,
	const Vec2& uvTiling, bool transparent, bool alphaTest,
	const Vec3& lightDirection, bool doubleSided, bool alphaOnlyTexture,
	bool overlay)
{
	return m_impl->drawRenderOperation(operation, worldViewProjection, color,
		textures, textureCount, uvTiling, transparent, alphaTest, lightDirection,
		doubleSided, alphaOnlyTexture, overlay);
}

void gkVkDeviceContext::resize(uint32 width, uint32 height)
{
	m_impl->requestedWidth = width;
	m_impl->requestedHeight = height;
	m_impl->isMinimized = width == 0 || height == 0;
	m_impl->resizePending = !m_impl->isMinimized;
}

void gkVkDeviceContext::setFullscreen(bool fullscreen)
{
	m_impl->setFullscreen(fullscreen);
}

HWND gkVkDeviceContext::window() const
{
	return m_impl->hwnd;
}

uint32 gkVkDeviceContext::width() const
{
	return m_impl->swapchainExtent.width;
}

uint32 gkVkDeviceContext::height() const
{
	return m_impl->swapchainExtent.height;
}

bool gkVkDeviceContext::minimized() const
{
	return m_impl->isMinimized;
}

float gkVkDeviceContext::gpuFrameTimeMs() const
{
	return m_impl->lastGpuFrameMs;
}
