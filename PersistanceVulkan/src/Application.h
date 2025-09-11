#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <vector>
#include <optional>

#include "DebugUtilsMessengerEXT.h"


#define BREAK __debugbreak();

const uint32_t screenwidth = 800;
const uint32_t screenheight = 600;

const std::vector<const char*> m_validationlayers = {
	"VK_LAYER_KHRONOS_validation"
};


#ifdef NDEBUG
const bool enablevalidationlayers = false;
#else
const bool enablevalidationlayers = true;
#endif

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsfamily;
	std::optional<uint32_t> computefamily;
	std::optional<uint32_t> transferfamily;


};


class Application
{
public:
	void run()
	{
		InitWindow();
		InitVulkan();
		MainLoop();
		CleanUp();

	}

private:

	void InitWindow()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);


		m_window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

		std::cout << "Initialized Window \n";
		



	}

	void InitVulkan()
	{
		CreateInstance();
		SetUpDebugCallBack();
		SelectPhysicalDevice();
		CreateLogicalDevice();

	}



	void MainLoop()
	{
		while (!glfwWindowShouldClose(m_window)) {
			glfwPollEvents();
		}




	}

	void CleanUp()
	{
		if (enablevalidationlayers) {
			//DestroyDebugUtilsMessengerEXT(m_instance, debugmessenger, nullptr);
			DebugUtilsMessengerEXT::Destroy(m_instance, debugmessenger, nullptr);
		}

		vkDestroyDevice(m_device, nullptr);

		vkDestroyInstance(m_instance, nullptr);

		glfwDestroyWindow(m_window);

		glfwTerminate();

	}

private:

	void SetUpDebugCallBack();

	void CreateInstance();

	bool CheckValidationLayers();

	std::vector<const char*> GetRequiredInstanceExtensions();

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallBack(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	void SetDebugCreateInfoStructVariables(VkDebugUtilsMessengerCreateInfoEXT& createinfo);

	void SelectPhysicalDevice();
	
	float RateDevice(VkPhysicalDevice& physicaldevice);

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice& physicaldevice);

	void CreateLogicalDevice();

private:

	GLFWwindow* m_window = nullptr;
	VkInstance m_instance = nullptr;
	VkDebugUtilsMessengerEXT debugmessenger;
	VkPhysicalDevice m_physicaldevice = VK_NULL_HANDLE;
	QueueFamilyIndices m_queuefamilyindices;
	VkDevice m_device;
	VkQueue m_graphicsqueue;

private:
	//bool IsDeviceSuitable(VkPhysicalDevice& physicaldevice);
	


};