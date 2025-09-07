#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
struct DebugUtilsMessengerEXT
{
	static VkResult Create(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* p_createinfo, const VkAllocationCallbacks* p_allocator, VkDebugUtilsMessengerEXT* p_debugmessenger)
	{
		

		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			std::cout << "DebugUtilsMessengerEXT created" << std::endl;
			return func(instance, p_createinfo, p_allocator, p_debugmessenger);
			
		}
		else
		{
			std::cout << "DebugUtilsMessengerEXT failed to create" << std::endl;
			return VK_ERROR_EXTENSION_NOT_PRESENT;
			

		}
	}
	static void Destroy(VkInstance instance, VkDebugUtilsMessengerEXT debugmessenger, const VkAllocationCallbacks* p_allocator) 
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugmessenger, p_allocator);
		}
		std::cout << "DebugUtilsMessengerEXT Destroyed" << std::endl;
	}

};