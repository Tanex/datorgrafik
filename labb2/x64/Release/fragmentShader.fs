/** fragmentShader.fs
    Example fragmentShader */

#version 330 

in vec4 position;
in vec4 colour;
in vec4 normal;
in vec2 textureCoordinate;

out vec4 fragmentColour;

uniform vec4 light0pos;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

const vec3 Ca = {0.8, 0.8, 0.5};
const vec3 Cd = {0.8, 0.8, 0.5};
const vec3 Cs = {1.0, 1.0, 1.0};
const vec3 Ia = {0.2, 0.2, 0.2};
const vec3 Id = {0.8, 0.8, 0.8};
const vec3 Is = {3.0, 3.0, 3.0};
const float f = 20.0; 

void main() {
    //vec4 Cd = texture(diffuseTexture, textureCoordinate);
    //vec4 Cs = texture(specularTexture, textureCoordinate);

    vec4 wi = normalize(light0pos - position);
    vec4 n = normalize(normal);
    vec4 wr = reflect(wi, n);//2*dot(wi,n)*n-wi;
    vec4 v = normalize(position);
    vec4 h = (wi+v)/length(wi+v);

    //fragmentColour = Ca*Ia + Cd*Id*dot(wi, n) + Cs*Is * pow(dot(h,n), f);
    fragmentColour.xyz = Ca*Ia + Cd*Id*dot(wi, n) + Cs*Is * pow(dot(h, n), f);
    fragmentColour.a = 0.0;

    //fragmentColour = colour;
}
