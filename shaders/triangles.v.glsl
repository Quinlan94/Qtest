#version 150


uniform mat4 u_pmv_matrix;


in vec3 a_position;

in vec3 a_normal;
in vec2 a_texcoord;

out vec2 v_texcoord;
out vec3 v_normal;


void main(void) 
{
  
  gl_Position = u_pmv_matrix * vec4(a_position, 1);
 
  v_normal = a_normal;
  v_texcoord= a_texcoord;

}
