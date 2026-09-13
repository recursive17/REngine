cmake_minimum_required(VERSION 3.12)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("REngine")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "VULKAN_SDK = $ENV{VULKAN_SDK}")
message(STATUS "Vulkan header = $ENV{VULKAN_SDK}/Include/vulkan/vulkan.h")
message(STATUS "Vulkan lib = $ENV{VULKAN_SDK}/Lib/vulkan-1.lib")

find_package(Vulkan REQUIRED)

message(STATUS "Vulkan_FOUND = ${Vulkan_FOUND}")
message(STATUS "Vulkan_INCLUDE_DIRS = ${Vulkan_INCLUDE_DIRS}")
message(STATUS "Vulkan_LIBRARY = ${Vulkan_LIBRARY}")
add_subdirectory("REngine")
