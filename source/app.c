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

#include "graphics.h"
#include "graphics.c"


APP_PERMANENT_LOAD// NOTE(Eric): INIT
{
    os = os_;
    GLS = &gls_;
    LoadAllOpenGLProcedures();
    
    // TODO(Eric): Clean this up and load/compile/createprogram in one step
    void* ShaderData = M_ArenaPush(&os->frame_arena, 2056);
    u64 ShaderLength = 0;
    String8 Path;
    Path.str = PATH_VS_3_CALC;
    Path.size = CalculateCStringLength(Path.str);
    os->LoadEntireFile(&os->frame_arena, Path, &ShaderData, &ShaderLength);
    Assert(ShaderLength > 0);
    GLS->vShader = GL_CreateShader(GL_VERTEX_SHADER, (char*)ShaderData);
    
    ShaderData = M_ArenaPushZero(&os->frame_arena, 2056);
    ShaderLength = 0;
    Path.str = PATH_FS_1;
    Path.size = CalculateCStringLength(Path.str);
    os->LoadEntireFile(&os->frame_arena, Path, &ShaderData, &ShaderLength);
    Assert(ShaderLength > 0);
    GLS->fShader = GL_CreateShader(GL_FRAGMENT_SHADER, (char*)ShaderData);
    
    GLS->theProgram = GL_CreateProgram();
    
    // NOTE(Eric): Query for the location of the Uniform within the program
    // NOTE(Eric): Returns -1 if it has no location
    GLS->uElapsedTime = glGetUniformLocation(GLS->theProgram, "time");
    
    GLS->uLoopDuration = glGetUniformLocation(GLS->theProgram, "loopDuration");
    
    glUseProgram(GLS->theProgram);
    glUniform1f(GLS->uLoopDuration, 5.0f);
    glUseProgram(0);
    
    // NOTE(Eric): Init Vertex Buffer
    {
        glGenBuffers(1, &GLS->vertexBufferObject);
        
        glBindBuffer(GL_ARRAY_BUFFER, GLS->vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glGenVertexArrays(1, &GLS->vao);
        glBindVertexArray(GLS->vao);
    }
    
    glViewport(0, 0, os->window_size.width, os->window_size.height);
}

APP_HOT_LOAD// NOTE(Eric): INIT AND ON CODE-RELOAD
{
    os = os_;
}

APP_HOT_UNLOAD {}

APP_UPDATE// NOTE(Eric): PER FRAME
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(GLS->theProgram);
    
    glUniform1f(GLS->uElapsedTime, os->GetTime());
    
	glBindBuffer(GL_ARRAY_BUFFER, GLS->vertexBufferObject);
	glEnableVertexAttribArray(0);// NOTE(Eric): This arg is the 'position=0' in the shader
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    
	glDrawArrays(GL_TRIANGLES, 0, 3);
    
	glDisableVertexAttribArray(0);
	glUseProgram(0);
    
    os->RefreshScreen();
}