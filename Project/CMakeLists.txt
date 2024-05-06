# Source files
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${SHADER_SOURCE_DIR}/*.frag"
    "${SHADER_SOURCE_DIR}/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${SHADER_BINARY_DIR}/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
)



set(SOURCES
    "main.cpp"
    "vulkanbase/VulkanBase.h" 
    "vulkanbase/VulkanUtil.h"
    # Add other source files here
    "labwork/Week01.cpp"
    "labwork/Week02.cpp" 
    "labwork/Week03.cpp" 
    "labwork/Week04.cpp" 
    "labwork/Week05.cpp"
    "labwork/Week06.cpp"
    "GP2Shader.cpp" "GP2CommandPool.cpp" "GP2CommandBuffer.cpp" "GP2Mesh.h" "GP2DataBuffer.cpp" "3DCamera.h" "GP2DescriptorPool.cpp" "GP2Utils.h" "GP2Pipeline.h")

    
   

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${GLSL_SOURCE_FILES}  "vulkanbase/VulkanUtil.cpp" "labwork/Week03.cpp" )
add_dependencies(${PROJECT_NAME} Shaders)
# Link libraries
#target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} glfw )
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} tinyobjloader )



set(RESOURCES_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Resources")
file(GLOB_RECURSE RESOURCE_FILES
    "${RESOURCES_SOURCE_DIR}/*.jpg"
    "${RESOURCES_SOURCE_DIR}/*.png"
    "${RESOURCES_SOURCE_DIR}/*.obj"
)

set(RESOURCES_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/Resources/")
file(MAKE_DIRECTORY ${RESOURCES_OUT_DIR})
foreach(RESOURCE ${RESOURCE_FILES})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${RESOURCE}
    ${RESOURCES_OUT_DIR})
endforeach(RESOURCE)


#target_include_directories(${SOURCE_DIR}/stb-master)
#include_directories(${SOURCE_DIR}/stb-master)	


