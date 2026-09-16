set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(REngine 
    "REngine/rengine.cpp"
    "REngine/rengine.hpp"
    "main.cpp"
    "REngine/helpers/device.cpp"
    "REngine/helpers/swapchain.cpp"
    "REngine/helpers/render_pass.cpp"
    "REngine/helpers/buffer.cpp"
    "REngine/helpers/command_context.cpp"
    "REngine/helpers/frame_context.cpp"
    "REngine/helpers/pipeline.cpp"
    "REngine/utils/helpers/recol.cpp"
    "REngine/utils/helpers/revec.cpp"
 "REngine/helpers/vma_impl.cpp" "REnder/renderer.cpp")

target_include_directories(REngine PRIVATE
    ${Vulkan_INCLUDE_DIRS}
)
target_include_directories(REngine PRIVATE
    ${CMAKE_SOURCE_DIR}/thirdparty/VMA/include
)

target_link_libraries(REngine PRIVATE
    Vulkan::Vulkan
)


if(WIN32)
    target_include_directories(REngine PRIVATE
        ${CMAKE_SOURCE_DIR}/thirdparty/GLFW/include
    )

    target_link_libraries(REngine PRIVATE
        ${CMAKE_SOURCE_DIR}/thirdparty/GLFW/lib/glfw3.lib
    )
elseif(UNIX)
    find_package(glfw3 REQUIRED)
    target_link_libraries(REngine PRIVATE glfw)
endif()


find_program(GLSLC glslc HINTS "$ENV{VULKAN_SDK}/Bin")
message(STATUS "GLSLC = ${GLSLC}")

file(GLOB_RECURSE SHADER_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/shaders/*.vert"
    "${CMAKE_SOURCE_DIR}/src/shaders/*.frag"
)

set(SPIRV_BINARY_DIR "${CMAKE_SOURCE_DIR}/src/shaders/compiled")
file(MAKE_DIRECTORY ${SPIRV_BINARY_DIR})

set(SPIRV_BINARY_FILES "")

foreach(SHADER ${SHADER_SOURCES})
    file(RELATIVE_PATH SHADER_REL "${CMAKE_SOURCE_DIR}/src/shaders" ${SHADER})
    get_filename_component(SHADER_DIR ${SHADER_REL} DIRECTORY)
    get_filename_component(SHADER_NAME_WE ${SHADER} NAME_WE)

    if(SHADER_DIR STREQUAL "")
        set(SPIRV_OUTPUT "${SPIRV_BINARY_DIR}/${SHADER_NAME_WE}.spv")
    else()
        set(SPIRV_OUTPUT "${SPIRV_BINARY_DIR}/${SHADER_DIR}/${SHADER_NAME_WE}.spv")
        file(MAKE_DIRECTORY "${SPIRV_BINARY_DIR}/${SHADER_DIR}")
    endif()

    add_custom_command(
        OUTPUT ${SPIRV_OUTPUT}
        COMMAND ${GLSLC} ${SHADER} -o ${SPIRV_OUTPUT}
        DEPENDS ${SHADER}
        COMMENT "Compiling shader ${SHADER_REL}"
    )

    list(APPEND SPIRV_BINARY_FILES ${SPIRV_OUTPUT})
endforeach()

add_custom_target(Shaders DEPENDS ${SPIRV_BINARY_FILES})
add_dependencies(REngine Shaders)

add_custom_command(TARGET REngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${SPIRV_BINARY_DIR}
    $<TARGET_FILE_DIR:REngine>/shaders
)
