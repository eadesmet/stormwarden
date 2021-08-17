#version 330

out vec4 outputColor;

void main()
{
	// A simple fragment shader using the resulting vertex Y position to base
	// the the output color on

    float lerpValue = gl_FragCoord.y / 500.0f;
    
    outputColor = mix(vec4(1.0f, 1.0f, 1.0f, 1.0f),
        vec4(0.2f, 0.2f, 0.2f, 1.0f), lerpValue);
}