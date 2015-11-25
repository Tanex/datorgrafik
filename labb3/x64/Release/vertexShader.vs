/** vertexShader.fs
    Example vertexShader */

#version 330

layout(location=0) in vec4 inPosition;
layout(location=1) in vec4 inColour;
layout(location=2) in vec4 inNormal;
//layout(location=3) in vec2 inTextureCoordinate;

out vec4 colour;
out vec4 position;
out vec4 normal;
//out vec2 textureCoordinate;

uniform mat4 projectionMatrix;
uniform mat4 modelviewMatrix;

void main() {
  //Don't apply perspective to position and normal
  //passed to the vertexShader
  position = modelviewMatrix * inPosition;
  normal = modelviewMatrix * inNormal;

  gl_Position = projectionMatrix * position;
  colour = inColour;
  //textureCoordinate = inTextureCoordinate;
}
