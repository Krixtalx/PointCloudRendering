#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in uint vColor;

uniform mat4 mModelViewProj;
uniform float pointSize;	

out vec4 ourColor;

void main() {
	gl_PointSize = pointSize;
	gl_Position = mModelViewProj * vec4(vPosition, 1.0);
	ourColor[0]=vColor&4278190080;
	ourColor[1]=vColor&16711680;
	ourColor[2]=vColor&65280;
	ourColor[3]=vColor&255;
}