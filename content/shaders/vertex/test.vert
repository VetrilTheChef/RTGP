#version 330 core

layout (location = 0) in vec3 position;

void main()
{
	gl_Position = vec4(position.x * 1.0f, position.y * 1.0f, position.z * 1.0f, 1.0);
}