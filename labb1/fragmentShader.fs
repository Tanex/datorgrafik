/** fragmentShader.fs
    Example fragmentShader */

#version 330

/* Accept RGB values from vertex shader, generate a corresponding RGBA values with alpha set to zero */
in vec3 vertexColour;
in vec3 position;
out vec4 fragmentColour;

void main() {
  // This is one way to make a vec4 (RGBA) vector out of a vec3 (RGB) vector
  fragmentColour = vec4(vertexColour, 0.0);

  // This would have been another one
  //fragmentColour.rgb = vertexColour;
  //fragmentColour.a = 0.0;

  // These would also work - but are confusing!
  // fragmentColour.xyz = vertexColour.rgb;
  // fragmentColour.zyx = vertexColour.bgr;
}
