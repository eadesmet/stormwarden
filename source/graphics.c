



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

internal m4
CalcStationaryOffset(f32 ElapsedTime)
{
    v3 Translation = {0.0f, 0.0f, -20.0f};
    
    m4 Result = M4TranslateV3(Translation);
    return Result;
}

internal m4
CalcOvalOffset(f32 ElapsedTime)
{
    f32 LoopDuration = 3.0f;
    f32 Scale = PI * 2.0f / LoopDuration;
    
    f32 CurrTimeThroughLoop = FMod(ElapsedTime, LoopDuration);
    
    v3 Translation = 
    {
        Cos(CurrTimeThroughLoop * Scale) * 4.0f,
        Sin(CurrTimeThroughLoop * Scale) * 6.0f,
        -20.0f,
    };
    
    m4 Result = M4TranslateV3(Translation);
    return Result;
}

internal m4
CalcBottomCircleOffset(f32 ElapsedTime)
{
    f32 LoopDuration = 3.0f;
    f32 Scale = PI * 2.0f / LoopDuration;
    
    f32 CurrTimeThroughLoop = FMod(ElapsedTime, LoopDuration);
    
    v3 Translation = 
    {
        Cos(CurrTimeThroughLoop * Scale) * 5.0f,
        -3.5f,
        Sin(CurrTimeThroughLoop * Scale) * 5.0f - 20.0f
    };
    
    m4 Result = M4TranslateV3(Translation);
    return Result;
}

internal f32
CalcFrustumScale(f32 FoV_Degrees)
{
    f32 DegreesToRadian = PI * 2 / 360;
    f32 FoV_Radians = FoV_Degrees * DegreesToRadian;
    
    f32 Result = 1.0f / Tan(FoV_Radians / 2.0f);
    return Result;
}

internal f32
CalcLerpFactor(f32 ElapsedTime, f32 LoopDuration)
{
    float Result = FMod(ElapsedTime, LoopDuration) / LoopDuration;
    if (Result > 0.5f)
        Result = 1.0f - Result;
    
    Result = Result * 2.0f;
    
    return Result;
}

enum SampleScale
{
    SAMPLESCALE_NULL,
    SAMPLESCALE_Uniform,
    SAMPLESCALE_NonUniform,
    SAMPLESCALE_DynamicUniform,
    SAMPLESCALE_DynamicNonUniform
} typedef SampleScale;
internal m4
SampleScales(f32 ElapsedTime, SampleScale WhichScale)
{
    // NOTE(Eric): All of these scale values have the same z-distance,
    // So the size difference is coming purely from the scaling.
    // The object in the center is unscaled (SAMPLESCALE_NULL).
    
    m4 Result;
    v3 Scale;
    v3 Offset;
    
    switch(WhichScale)
    {
        case SAMPLESCALE_NULL:
        {
            Scale = v3(1.0f, 1.0f, 1.0f);
            Offset = v3(0.0f, 0.0f, -45.0f);
            break;
        }
        case SAMPLESCALE_Uniform:
        {
            Scale = v3(4.0f, 4.0f, 4.0f);
            Offset = v3(-10.0f, -10.0f, -45.0f);
            break;
        }
        case SAMPLESCALE_NonUniform:
        {
            Scale = v3(0.5f, 1.0f, 10.0f);
            Offset = v3(-10.0f, 10.0f, -45.0f);
            break;
        }
        case SAMPLESCALE_DynamicUniform:
        {
            f32 LoopDuration = 3.0f;
            
            Scale = v3(1.0f, 4.0f, CalcLerpFactor(ElapsedTime, LoopDuration));
            Offset = v3(10.0f, 10.0f, -45.0f);
            break;
        }
        case SAMPLESCALE_DynamicNonUniform:
        {
            f32 LoopDurationX = 3.0f;
            f32 LoopDurationZ = 5.0f;
            
            f32 X = Lerp(1.0f, 0.5f, CalcLerpFactor(ElapsedTime, LoopDurationX));
            f32 Y = 1.0f;
            f32 Z = Lerp(1.0f, 10.0f, CalcLerpFactor(ElapsedTime, LoopDurationZ));
            
            Scale = v3(X, Y, Z);
            Offset = v3(10.0f, -10.0f, -45.0f);
            break;
        }
    }
    
    Result = M4ScaleV3(Scale);
    
    Result.elements[3][0] = Offset.x;
    Result.elements[3][1] = Offset.y;
    Result.elements[3][2] = Offset.z;
    Result.elements[3][3] = 1.0f;
    
    return(Result);
}

//

internal f32
ComputeAngleRad(f32 ElapsedTime, f32 LoopDuration)
{
    f32 Scale = PI * 2.0f / LoopDuration;
    f32 CurrTimeThroughLoop = FMod(ElapsedTime, LoopDuration);
    f32 Result = CurrTimeThroughLoop * Scale;
    
    return(Result);
}

internal m4
RotateX(f32 ElapsedTime)
{
    f32 AngRad = ComputeAngleRad(ElapsedTime, 3.0);
    f32 fCos = Cos(AngRad);
    f32 fSin = Sin(AngRad);
    
    m4 Result = M4InitD(1.0f);
    Result.elements[1][1] = fCos;
    Result.elements[2][1] = -fSin;
    Result.elements[1][2] = fSin;
    Result.elements[2][2] = fCos;
    
    return(Result);
}

internal m4
RotateY(f32 ElapsedTime)
{
    f32 AngRad = ComputeAngleRad(ElapsedTime, 2.0);
    f32 fCos = Cos(AngRad);
    f32 fSin = Sin(AngRad);
    
    m4 Result = M4InitD(1.0f);
    Result.elements[0][0] = fCos;
    Result.elements[2][0] = fSin;
    Result.elements[0][2] = -fSin;
    Result.elements[2][2] = fCos;
    
    return(Result);
}

internal m4
RotateZ(f32 ElapsedTime)
{
    f32 AngRad = ComputeAngleRad(ElapsedTime, 2.0);
    f32 fCos = Cos(AngRad);
    f32 fSin = Sin(AngRad);
    
    m4 Result = M4InitD(1.0f);
    Result.elements[0][0] = fCos;
    Result.elements[1][0] = -fSin;
    Result.elements[0][1] = fSin;
    Result.elements[1][1] = fCos;
    
    return(Result);
}

internal m4
RotateAxis(f32 ElapsedTime)
{
    f32 AngRad = ComputeAngleRad(ElapsedTime, 2.0);
    f32 fCos = Cos(AngRad);
    f32 fInvCos = 1.0f - fCos;
    f32 fSin = Sin(AngRad);
    f32 fInvSin = 1.0f - fSin;
    
    v3 Axis = v3(1.0f, 1.0f, 1.0f);
    Axis = V3Normalize(Axis);
    
    m4 Result = M4InitD(1.0f);
    
    Result.elements[0][0] = (Axis.x * Axis.x) + ((1 - Axis.x * Axis.x) * fCos);
	Result.elements[1][0] = Axis.x * Axis.y * (fInvCos) - (Axis.z * fSin);
	Result.elements[2][0] = Axis.x * Axis.z * (fInvCos) + (Axis.y * fSin);
    
	Result.elements[0][1] = Axis.x * Axis.y * (fInvCos) + (Axis.z * fSin);
	Result.elements[1][1] = (Axis.y * Axis.y) + ((1 - Axis.y * Axis.y) * fCos);
	Result.elements[2][1] = Axis.y * Axis.z * (fInvCos) - (Axis.x * fSin);
    
	Result.elements[0][2] = Axis.x * Axis.z * (fInvCos) - (Axis.y * fSin);
	Result.elements[1][2] = Axis.y * Axis.z * (fInvCos) + (Axis.x * fSin);
	Result.elements[2][2] = (Axis.z * Axis.z) + ((1 - Axis.z * Axis.z) * fCos);
    
    return(Result);
}

enum SampleRotation
{
    SAMPLEROTATION_NULL,
    SAMPLEROTATION_RotateX,
    SAMPLEROTATION_RotateY,
    SAMPLEROTATION_RotateZ,
    SAMPLEROTATION_RotateAxis
} typedef SampleRotation;
internal m4
SampleRotations(f32 ElapsedTime, SampleRotation WhichRotation)
{
    m4 Result;
    v3 Scale;
    v3 Offset;
    
    switch(WhichRotation)
    {
        case SAMPLEROTATION_NULL:
        {
            Result = M4InitD(1.0f);
            Offset = v3(0.0f, 0.0f, -25.0f);
            break;
        }
        case SAMPLEROTATION_RotateX:
        {
            Result = RotateX(ElapsedTime);
            Offset = v3(-5.0f, -5.0f, -25.0f);
            break;
        }
        case SAMPLEROTATION_RotateY:
        {
            Result = RotateY(ElapsedTime);
            Offset = v3(-5.0f, 5.0f, -25.0f);
            break;
        }
        case SAMPLEROTATION_RotateZ:
        {
            Result = RotateZ(ElapsedTime);
            Offset = v3(5.0f, 5.0f, -25.0f);
            break;
        }
        case SAMPLEROTATION_RotateAxis:
        {
            Result = RotateAxis(ElapsedTime);
            Offset = v3(5.0f, -5.0f, -25.0f);
            break;
        }
    }
    
    Result.elements[3][0] = Offset.x;
    Result.elements[3][1] = Offset.y;
    Result.elements[3][2] = Offset.z;
    Result.elements[3][3] = 1.0f;
    
    return(Result);
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