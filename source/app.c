#include "language_layer.h"
#include "maths.h"
#include "memory.h"
#include "strings.h"
#include "perlin.h"
#include "os.h"
#include "opengl.h"

#include "language_layer.c"
#include "maths.c"
#include "memory.c"
#include "strings.c"
#include "perlin.c"
#include "os.c"

// TODO(Eric): OpenGL Garbage for testing! Move this somewhere else!
GLuint vShader;
GLuint fShader;
GLuint theProgram;
GLuint positionBufferObject;
GLuint vao;

const char* strVertexShader =
"#version 330\n"\
"layout(location = 0) in vec4 position;\n"\
"void main()\n"\
"{\n"\
"   gl_Position = position;\n"\
"}\n";

const char* strFragmentShader = 
"#version 330\n"\
"out vec4 outputColor;\n"\
"void main()\n"\
"{\n"\
"   outputColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"\
"}\n";

const float vertexPositions[] = {
	0.75f, 0.75f, 0.0f, 1.0f,
	0.75f, -0.75f, 0.0f, 1.0f,
	-0.75f, -0.75f, 0.0f, 1.0f,
};

GLuint CreateShader(GLenum eShaderType, const char* strFileData)
{
	GLuint shader = glCreateShader(eShaderType);
	glShaderSource(shader, 1, &strFileData, NULL);
    
	glCompileShader(shader);
    
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        
		GLchar strInfoLog[256];//[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
        
		const char *strShaderType = NULL;
		switch(eShaderType)
		{
            case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
            case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
            case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}
        
		fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
        //free(strInfoLog);
	}
    
	return shader;
}

GLuint CreateProgram()
{
	GLuint program = glCreateProgram();
    
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    
	glLinkProgram(program);
    
	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        
		GLchar strInfoLog[256];//[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		//delete[] strInfoLog;
	}
    
    glDetachShader(program, vShader);
    glDetachShader(program, fShader);
    
    glDeleteShader(vShader);
    glDeleteShader(fShader);
    
	return program;
}



APP_PERMANENT_LOAD// NOTE(Eric): INIT
{
    os = os_;
    LoadAllOpenGLProcedures();
    
    // TODO(Eric): Do all this in 1 step since all we need is the program
    vShader = CreateShader(GL_VERTEX_SHADER, strVertexShader);
    fShader = CreateShader(GL_FRAGMENT_SHADER, strFragmentShader);
    theProgram = CreateProgram();
    
    // NOTE(Eric): Init Vertex Buffer
    {
        glGenBuffers(1, &positionBufferObject);
        
        glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
    }
}

APP_HOT_LOAD// NOTE(Eric): INIT AND ON CODE-RELOAD
{
    os = os_;
}

APP_HOT_UNLOAD {}

APP_UPDATE// NOTE(Eric): PER FRAME
{
    
    
    glClearColor(0.2f, 0.4f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(theProgram);
    
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glEnableVertexAttribArray(0);// NOTE(Eric): This arg is the 'position=0' in the shader
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    
	glDrawArrays(GL_TRIANGLES, 0, 3);
    
	glDisableVertexAttribArray(0);
	glUseProgram(0);
    
    os->RefreshScreen();
}