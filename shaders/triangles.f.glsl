#version 150


uniform sampler2D texture_tri;


uniform vec4      ambientColor;
uniform vec4      diffuseColor;   
uniform vec4      specularColor;


in vec2 v_texcoord;
in vec3 v_normal;
in vec3 vVaryingLightDir;

out vec4 FragColor; 

void main(void) 
{
  
   float diff = max(0.0, dot(normalize(v_normal), normalize(vVaryingLightDir)));
   FragColor = diff * diffuseColor;
   FragColor += ambientColor;
   FragColor *= texture(texture_tri, v_texcoord);


}

