#version 150

uniform sampler2D texture;

in vec2 v_texcoord;

void main(void) {
  gl_FragColor = texture2D(texture, v_texcoord);
}
