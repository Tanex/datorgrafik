/** vertexShader.fs
    Example vertexShader */

#version 330

layout(location=0) in vec4 inPosition;
layout(location=1) in vec4 inColour;

out vec4 colour;

void main() {
  gl_Position = inPosition;
  colour = inColour;
}
