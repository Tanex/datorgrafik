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

const vec4 Ca = vec4(0.8, 0.8, 0.5, 0.0);
//const vec4 Cd = vec4(0.8, 0.8, 0.5, 0.0);
//const vec4 Cs = vec4(1.0, 1.0, 1.0, 0.0);
const vec4 Ia = vec4(0.2, 0.2, 0.2, 0.0);
const vec4 Id = vec4(0.8, 0.8, 0.8, 0.0);
const vec4 Is = vec4(3.0, 3.0, 3.0, 0.0);
const float f = 80.0; 

void main() {
    vec4 Cd = texture(diffuseTexture, textureCoordinate);
    vec4 Cs = texture(specularTexture, textureCoordinate);
    
    vec4 wi = normalize(light0pos - position);
    vec4 n = normalize(normal);

    float diffuse = max(dot(wi, n), 0.0);
    float specular = 0.0;

    if (diffuse > 0.0)
    {        
        vec4 v = normalize(position);

        //Blinn-Phong
       vec4 h = normalize(wi-v);
       specular = pow(max(dot(h, n), 0.0), f);
        
        //Phong
        //vec4 wr = reflect(wi, n);//2*dot(wi,n)*n-wi;
        //specular = pow(max(dot(wr, v), 0.0), f);///4.0);
    }

    fragmentColour = Ca*Ia + Cd*Id*diffuse + Cs*Is*specular;
    //fragmentColour.xyz = Ca*Ia + Cd*Id*dot(wi, n) + Cs*Is * pow(max(dot(wr, v), 0.0), f);


    //fragmentColour = colour;
}
