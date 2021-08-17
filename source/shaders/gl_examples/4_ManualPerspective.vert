#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

smooth out vec4 theColor;

uniform vec2 offset;
uniform float zNear;
uniform float zFar;
uniform float frustumScale;


void main()
{
	// This first statement simply applies an offset, just like we have seen before.
	// It positions the object in camera space, so that it is offset from the center of the view.
	// This is here to make it easier to position the object for projection
	vec4 cameraPos = position + vec4(offset.x, offset.y, 0.0, 0.0);
	vec4 clipPos;
	
	// This performs a scalar multiply of the camera-space X and Y positions, storing them in a temp variable.
	clipPos.xy = cameraPos.xy * frustumScale;
	
	// Computing the clip Z position based on the depth computation formula
	clipPos.z = cameraPos.z * (zNear + zFar) / (zNear - zFar);
	clipPos.z += 2 * zNear * zFar / (zNear - zFar);
	
	// The W coordinate of the clip space position is the Z distance in camera space 
	// divided by the Z distance from the plane (at the origin) to the eye.
	// The eye is fixed at (0, 0, -1), so this leaves us with the negation of the camera space Z position.
	// OpenGL will automatically perform the division for us.
	clipPos.w = -cameraPos.z;
	
	gl_Position = clipPos;
	theColor = color;
}
