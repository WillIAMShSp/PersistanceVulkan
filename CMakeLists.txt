cmake_minimum_required(VERSION 3.23)

project(PersistanceVulkan)

#Requires C++ 17 standard.
set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

############################
# Uncomment this to build test application.
#
#add_executable(PersistanceExecutableTest)

#target_sources( PersistanceExecutableTest
#PRIVATE
#	"./PersistanceVulkan/src/PersistanceVulkan.cpp"
#)

file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS
	"./PersistanceVulkan/src/**/*.cpp"
	"./PersistanceVulkan/src/**/*.c"
	)

add_library(PersistanceLibrary STATIC ${LIB_SOURCES})

target_include_directories(PersistanceLibrary PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_include_directories(PersistanceLibrary PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include/Vulkan)
target_include_directories(PersistanceLibrary PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/PersistanceVulkan/src) 

target_link_libraries(PersistanceLibrary PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/Lib/GLFW/glfw3_mt.lib
gdi32
user32
shell32
)

target_link_libraries(PersistanceExecutableTest PRIVATE PersistanceLibrary) 



