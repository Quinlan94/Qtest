#version 150 core

uniform mat4 u_pmv_matrix;

in vec3 a_position;
#in vec4 a_color;
in vec3 a_normal;
in vec2 a_texcoord;

out vec3 v_normal;
#out vec4 v_color;
out vec2 v_texcoord;


void main(void)
{
  gl_Position = u_pmv_matrix * vec4(a_position, 1);
#v_color = a_color;
  v_normal = a_normal;
  v_texcoord = a_texcoord;
}
