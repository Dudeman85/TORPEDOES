#version 330 core

in vec3 a_position;
in vec2 a_texCoord;

uniform mat4 u_projectionMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_modelMatrix;

out vec2 v_texCoord;

void main()
{
	gl_Position =  u_projectionMatrix * u_viewMatrix * u_modelMatrix * vec4(a_position, 1.0);
	//gl_Position = vec4(a_position, 1.0);

	v_texCoord = a_texCoord;
}