#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

smooth out vec4 theColor;

uniform vec3 offset;
uniform mat4 perspectiveMatrix;

// Notice that this one passes in the Matrix instead of doing the computation here
// This is to save precious vertex shader time doing redundant computations.
// (because all the objects of the scene will be rendered using the same perspective matrix)

void main()
{
    vec4 cameraPos = position + vec4(offset.x, offset.y, offset.z, 0.0);
    
	// Vector-matrix multiplication is so common that it's simply used with a *
	// Note the _order_ of this operation: the matrix is on the left, vector on the right.
	// Matrix multiplication is _not_ commutative, so M*v is not the same thing as v*M
	// Here we want to multiply rows of the matrix by the vector, not columns of the matrix.
    gl_Position = perspectiveMatrix * cameraPos;
    theColor = color;
}