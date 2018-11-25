#version 150


uniform mat4 u_pmv_matrix;

uniform mat4 tri_model_matrix_;
uniform mat4 tri_view_matrix_;
uniform mat3 normal_matrix_;
uniform vec3 vLightPosition;


in vec3 a_position;

in vec3 a_normal;
in vec2 a_texcoord;

out vec2 v_texcoord;
out vec3 v_normal;
out vec3 vVaryingLightDir;



void main(void) 
{
 

 
  gl_Position = u_pmv_matrix * vec4(a_position, 1);
  v_normal = normal_matrix_ * a_normal;

  vec4 vPosition4 = tri_view_matrix_ * tri_model_matrix_ * vec4(a_position, 1);

  vec3 vPosition3 = vPosition4.xyz / vPosition4.w;

  vVaryingLightDir = normalize(vLightPosition - vPosition3);

   
  
  v_texcoord = a_texcoord;


  
}
