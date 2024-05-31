#version 450
layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0)in vec3 inPosition;
layout(location = 1)in vec3 inColor;
layout(location = 2)in vec2 inTexCoord;

layout(location = 3)in vec4 modelC0;
layout(location = 4)in vec4 modelC1;
layout(location = 5)in vec4 modelC2;
layout(location = 6)in vec4 modelC3;
layout(location = 7)in vec2 texCoordOffset;
// Use the attributes to construct a mat4
mat4 model = mat4(modelC0 , modelC1 , modelC2 , modelC3);


layout(location = 0)out vec3 fragColor;
layout(location = 1)out vec2 fragTexCoord;

void main(){
gl_Position = ubo.proj* ubo.view* model* vec4(inPosition.xyz , 1);

fragColor = inColor;
// 32 pixels width and height of cell in texture atlas.
vec2 cellSize = vec2(1.0 / 16.0, 1.0 / 16.0);
fragTexCoord = texCoordOffset * 32.f + inTexCoord;
  //texCoordOffset * cellSize + inTexCoord / 32.f;
}