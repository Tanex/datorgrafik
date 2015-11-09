/** vertexShader.fs
    Example vertexShader */

#version 330

layout(location=0) in vec4 inPosition;
layout(location=1) in vec3 inColour;
out vec3 vertexColour;
out vec3 position;

void main() {
  gl_Position = inPosition;
  vertexColour = inColour;
  position = inPosition.xyz;
}
