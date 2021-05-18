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
    Path.str = PATH_VS_2;
    Path.size = CalculateCStringLength(Path.str);
    os->LoadEntireFile(&os->frame_arena, Path, &ShaderData, &ShaderLength);
    GLS->vShader = GL_CreateShader(GL_VERTEX_SHADER, (char*)ShaderData);
    
    ShaderData = M_ArenaPushZero(&os->frame_arena, 2056);
    ShaderLength = 0;
    Path.str = PATH_FS_2;
    Path.size = CalculateCStringLength(Path.str);
    os->LoadEntireFile(&os->frame_arena, Path, &ShaderData, &ShaderLength);
    GLS->fShader = GL_CreateShader(GL_FRAGMENT_SHADER, (char*)ShaderData);
    
    GLS->theProgram = GL_CreateProgram();
    
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
    // NOTE(Eric): Calculate the offset X and Y
    f32 OffsetX, OffsetY;
    {
        const float fLoopDuration = 5.0f;
        const float fScale = 3.14159f * 2.0f / fLoopDuration;
        
        float fElapsedTime = os->GetTime() / 10.0f;
        
        float fCurrTimeThroughLoop = fmodf(fElapsedTime, fLoopDuration);
        
        OffsetX = Cos(fCurrTimeThroughLoop * fScale) * 0.5f;
        OffsetY = Sin(fCurrTimeThroughLoop * fScale) * 0.5f;
    }
    
    // NOTE(Eric): Update the vertex data
    {
        float NewVertexData[ArrayCount(vertexData)];
        memcpy(&NewVertexData, vertexData, sizeof(vertexData));
        
        for (u32 Index = 0; Index < ArrayCount(vertexData); Index += 4)
        {
            NewVertexData[Index] += OffsetX;
            NewVertexData[Index+1] += OffsetY;
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, GLS->vertexBufferObject);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexData), &NewVertexData);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(GLS->theProgram);
    
	glBindBuffer(GL_ARRAY_BUFFER, GLS->vertexBufferObject);
	glEnableVertexAttribArray(0);// NOTE(Eric): This arg is the 'position=0' in the shader
    glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)48);
    
	glDrawArrays(GL_TRIANGLES, 0, 3);
    
	glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
	glUseProgram(0);
    
    os->RefreshScreen();
}