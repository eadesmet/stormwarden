#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

smooth out vec4 theColor;

uniform mat4 CameraToClipMatrix;
uniform mat4 ModelToCameraMatrix;

void main()
{
	vec4 cameraPos = ModelToCameraMatrix * position;
	gl_Position = CameraToClipMatrix * cameraPos;
	theColor = color;
}
