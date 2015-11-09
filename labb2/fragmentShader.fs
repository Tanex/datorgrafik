/** fragmentShader.fs
    Example fragmentShader */

#version 330 

in vec4 position;
in vec4 colour;

out vec4 fragmentColour;

void main() {
  fragmentColour = colour;
}
