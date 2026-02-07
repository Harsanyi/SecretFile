#version 330

in vec3 vertexPosition;
in vec3 vertexNormal;

uniform mat4 mvp;
uniform mat4 matModel;

out vec3 fragNormal;

void main()
{
	mat3 normalMatrix = transpose(inverse(mat3(matModel)));
	fragNormal = normalize(normalMatrix * vertexNormal);
	gl_Position = mvp * vec4(vertexPosition, 1.0);
}