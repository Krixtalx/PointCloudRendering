#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;

uniform mat4 mModelViewProj;
uniform float pointSize;	

out vec4 ourColor;

void main() {
	gl_PointSize = pointSize;
	gl_Position = mModelViewProj * vec4(vPosition, 1.0);
	ourColor=vec4(vColor, 1.0);
}