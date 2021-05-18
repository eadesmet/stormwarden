



internal GLuint 
GL_CreateShader(GLenum eShaderType, const char* strFileData)
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
        
		LogError("Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
        //free(strInfoLog);
	}
    
	return shader;
}

internal GLuint 
GL_CreateProgram()
{
	GLuint program = glCreateProgram();
    
    glAttachShader(program, GLS->vShader);
    glAttachShader(program, GLS->fShader);
    
	glLinkProgram(program);
    
	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        
		GLchar strInfoLog[256];//[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		LogError("Linker failure: %s\n", strInfoLog);
		//delete[] strInfoLog;
	}
    
    glDetachShader(program, GLS->vShader);
    glDetachShader(program, GLS->fShader);
    
    glDeleteShader(GLS->vShader);
    glDeleteShader(GLS->fShader);
    
    return program;
}















#if 0

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
"   outputColor = vec4(0.7f, 0.7f, 1.0f, 1.0f);\n"\
"}\n";

#endif