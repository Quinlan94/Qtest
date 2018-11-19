#version 150


uniform sampler2D texture_tri;


in vec2 v_texcoord;


out vec4 FragColor; 
void main(void) 
{
  
   FragColor = texture(texture_tri, v_texcoord);

}
