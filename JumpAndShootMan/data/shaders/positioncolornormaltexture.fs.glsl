#version 330 core

uniform sampler2D uTextureSampler;

in vec4 vs_fs_color;
in vec3 vs_fs_normal;
in vec2 vs_fs_texcoord;

void main()
{
	gl_FragColor = texture2D(uTextureSampler, vs_fs_texcoord) * vs_fs_color;
}