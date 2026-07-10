cmake_minimum_required(VERSION 3.23)

project(PersistanceVulkan)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")


add_executable(PersistanceExecutable)

target_sources( PersistanceExecutable
PRIVATE
	"./PersistanceVulkan/src/PersistanceVulkan.cpp"
)

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

target_link_libraries(PersistanceExecutable PRIVATE PersistanceLibrary)


