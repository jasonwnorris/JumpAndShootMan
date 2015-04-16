#version 330 core

uniform mat4 uProjectionMatrix;
uniform mat4 uModelViewMatrix;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec4 Color;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec2 TexCoord;

out vec4 vs_fs_color;
out vec3 vs_fs_normal;
out vec2 vs_fs_texcoord;

void main()
{
	vs_fs_color = Color;
	vs_fs_normal = Normal;
	vs_fs_texcoord = TexCoord;

	gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(Position, 1.0);
}