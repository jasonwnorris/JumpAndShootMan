#version 330 core

in vec4 vs_fs_color;

void main()
{
	gl_FragColor = vs_fs_color;
}