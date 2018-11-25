#version 300 es
#extension GL_OES_EGL_image_external_essl3 : require
precision mediump float;
uniform samplerExternalOES uTextureSampler;
in vec2 vTextureCoord;
out vec4 vFragColor;
void main()
{
  vFragColor = texture(uTextureSampler, vTextureCoord);
}
