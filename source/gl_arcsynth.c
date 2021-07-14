



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

// NOTE(Eric): This is a stupid hack. Somehow even though it's loading this procedure in
// LoadAllOpenGLProcedures(), it's still not resolving the external symbol?
// But when called manually, it works. I DON'T KNOW MAN.
// ALSO not available from the gl_core_46.c files, generated from galogen
// Is it from some extension??? Maybe a problem with my machine's driver??
void (*GL_ClearDepthF)(float);



// APP_PERMANENT_LOAD
GLS = &gls_;
//LoadAllOpenGLProcedures();
GL_ClearDepthF = os->LoadOpenGLProcedure("glClearDepthf");

// TODO(Eric): Clean this up and load/compile/createprogram in one step
void* ShaderData = M_ArenaPush(&os->frame_arena, 2056);
u64 ShaderLength = 0;
String8 Path;
Path.str = PATH_VS_6;
Path.size = CalculateCStringLength(Path.str);
os->LoadEntireFile(&os->frame_arena, Path, &ShaderData, &ShaderLength);
Assert(ShaderLength > 0);
GLS->vShader = GL_CreateShader(GL_VERTEX_SHADER, (char*)ShaderData);

ShaderData = M_ArenaPushZero(&os->frame_arena, 2056);
ShaderLength = 0;
Path.str = PATH_FS_6;
Path.size = CalculateCStringLength(Path.str);
os->LoadEntireFile(&os->frame_arena, Path, &ShaderData, &ShaderLength);
Assert(ShaderLength > 0);
GLS->fShader = GL_CreateShader(GL_FRAGMENT_SHADER, (char*)ShaderData);

GLS->theProgram = GL_CreateProgram();

GLS->ModelToCameraMatrixUnif = glGetUniformLocation(GLS->theProgram, "ModelToCameraMatrix");
GLS->CameraToClipMatrixUnif = glGetUniformLocation(GLS->theProgram, "CameraToClipMatrix");

fFrustumScale = CalcFrustumScale(45.0f);
f32 fzNear = 1.0f;
f32 fzFar = 61.0f;

m4 m = {0};
GLS->CameraToClipMatrix = m;
GLS->CameraToClipMatrix.elements[0][0] = fFrustumScale;
GLS->CameraToClipMatrix.elements[1][1] = fFrustumScale;
GLS->CameraToClipMatrix.elements[2][2] = (fzFar + fzNear) / (fzNear - fzFar);
GLS->CameraToClipMatrix.elements[2][3] = -1.0f;
GLS->CameraToClipMatrix.elements[3][2] = (2 * fzFar * fzNear) / (fzNear - fzFar);

glUseProgram(GLS->theProgram);
glUniformMatrix4fv(GLS->CameraToClipMatrixUnif, 1, GL_FALSE, &GLS->CameraToClipMatrix.elements[0][0]);
glUseProgram(0);

// NOTE(Eric): Init Vertex Buffer
{
    glGenBuffers(1, &GLS->vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, GLS->vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glGenBuffers(1, &GLS->indexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GLS->indexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// NOTE(Eric): Init Vertex Array Objects
{
    glGenVertexArrays(1, &GLS->vao);
    glBindVertexArray(GLS->vao);
    
    size_t colorDataOffset = sizeof(float) * 3 * numberOfVertices;
    
    glBindBuffer(GL_ARRAY_BUFFER, GLS->vertexBufferObject);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)colorDataOffset);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GLS->indexBufferObject);
    
    glBindVertexArray(0);
}

glEnable(GL_CULL_FACE);
glCullFace(GL_BACK);
glFrontFace(GL_CW);

glEnable(GL_DEPTH_TEST);
glDepthMask(GL_TRUE);
glDepthFunc(GL_LEQUAL);
glDepthRange(0.0f, 1.0f);
#endif


#if 0
// APP_UPDATE
if (os->resized)
{
    GLS->CameraToClipMatrix.elements[0][0] = fFrustumScale / (os->window_size.width / (float)os->window_size.height);
    GLS->CameraToClipMatrix.elements[1][1] = fFrustumScale;
    
    glUseProgram(GLS->theProgram);
    glUniformMatrix4fv(GLS->CameraToClipMatrixUnif, 1, GL_FALSE, &GLS->CameraToClipMatrix.elements[0][0]);
    glUseProgram(0);
    
    glViewport(0, 0, (GLsizei)os->window_size.width, (GLsizei)os->window_size.height);
    
    os->resized = 0;
}

local_persist b32 DepthClampingActive = 0;
if (os->event_count > 0)
{
    // NOTE(Eric): Not sure if this is how I'm supposed to handle events in the app,
    // but it's working for now.
    for (u32 EventIndex = 0;
         EventIndex < os->event_count;
         EventIndex++)
    {
        OS_Event *Event = 0;
        OS_GetNextEvent(&Event);
        if(!Event) break;
        
        if (Event->type == OS_EventType_KeyPress)
        {
            switch(Event->key)
            {
                case Key_Space:
                {
                    if (DepthClampingActive)
                        glDisable(GL_DEPTH_CLAMP);
                    else
                        glEnable(GL_DEPTH_CLAMP);
                    
                    DepthClampingActive = !DepthClampingActive;
                }break;
                case Key_Esc:
                {
                    os->quit = 1;
                }break;
                default:break;
            }
        }
        
        OS_EatEvent(Event);
    }
}

glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
GL_ClearDepthF(1.0f);//glCearDepthf(1.0f);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

glUseProgram(GLS->theProgram);

glBindVertexArray(GLS->vao);

float ElapsedTime = os->GetTime();
{
    m4 NullRotation = SampleRotations(ElapsedTime, SAMPLEROTATION_NULL);
    glUniformMatrix4fv(GLS->ModelToCameraMatrixUnif, 1, GL_FALSE, &NullRotation.elements[0][0]);
    glDrawElements(GL_TRIANGLES, ArrayCount(indexData), GL_UNSIGNED_SHORT, 0);
    
    m4 XRotation = SampleRotations(ElapsedTime, SAMPLEROTATION_RotateX);
    glUniformMatrix4fv(GLS->ModelToCameraMatrixUnif, 1, GL_FALSE, &XRotation.elements[0][0]);
    glDrawElements(GL_TRIANGLES, ArrayCount(indexData), GL_UNSIGNED_SHORT, 0);
    
    m4 YRotation = SampleRotations(ElapsedTime, SAMPLEROTATION_RotateY);
    glUniformMatrix4fv(GLS->ModelToCameraMatrixUnif, 1, GL_FALSE, &YRotation.elements[0][0]);
    glDrawElements(GL_TRIANGLES, ArrayCount(indexData), GL_UNSIGNED_SHORT, 0);
    
    m4 ZRotation = SampleRotations(ElapsedTime, SAMPLEROTATION_RotateZ);
    glUniformMatrix4fv(GLS->ModelToCameraMatrixUnif, 1, GL_FALSE, &ZRotation.elements[0][0]);
    glDrawElements(GL_TRIANGLES, ArrayCount(indexData), GL_UNSIGNED_SHORT, 0);
    
    m4 AxisRotation = SampleRotations(ElapsedTime, SAMPLEROTATION_RotateAxis);
    glUniformMatrix4fv(GLS->ModelToCameraMatrixUnif, 1, GL_FALSE, &AxisRotation.elements[0][0]);
    glDrawElements(GL_TRIANGLES, ArrayCount(indexData), GL_UNSIGNED_SHORT, 0);
}


glBindVertexArray(0);
glUseProgram(0);

#endif



















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