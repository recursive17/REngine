add_executable (REngine "REngine/REngine.cpp" "REngine/REngine.hpp" "main.cpp"
    "REngine/helpers/device.cpp" "REngine/helpers/swapchain.cpp"
    "REngine/helpers/render_pass.cpp" "REngine/helpers/buffer.cpp"
    "REngine/helpers/command_context.cpp" "REngine/helpers/frame_context.cpp"
    "REngine/helpers/pipeline.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET REngine PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(REngine PRIVATE Vulkan::Vulkan)

target_include_directories(REngine PRIVATE
    ${CMAKE_SOURCE_DIR}/thirdparty/glm
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

add_custom_command(TARGET REngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src/shaders
    $<TARGET_FILE_DIR:REngine>/shaders
)