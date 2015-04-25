#version 330 core

uniform mat4 uProjectionMatrix;
uniform mat4 uModelViewMatrix;

layout (location = 0) in vec2 Position;
layout (location = 1) in vec4 Color;

out vec4 vs_fs_color;

void main()
{
	vs_fs_color = Color;

	gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(Position, 1.0, 1.0);
}