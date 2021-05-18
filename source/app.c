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
    Path.str = PATH_VS_4_MANUAL_PERSPECTIVE;
    Path.size = CalculateCStringLength(Path.str);
    os->LoadEntireFile(&os->frame_arena, Path, &ShaderData, &ShaderLength);
    Assert(ShaderLength > 0);
    GLS->vShader = GL_CreateShader(GL_VERTEX_SHADER, (char*)ShaderData);
    
    ShaderData = M_ArenaPushZero(&os->frame_arena, 2056);
    ShaderLength = 0;
    Path.str = PATH_FS_4_ORTHO_CUBE;
    Path.size = CalculateCStringLength(Path.str);
    os->LoadEntireFile(&os->frame_arena, Path, &ShaderData, &ShaderLength);
    Assert(ShaderLength > 0);
    GLS->fShader = GL_CreateShader(GL_FRAGMENT_SHADER, (char*)ShaderData);
    
    GLS->theProgram = GL_CreateProgram();
    
    GLS->offsetUniform = glGetUniformLocation(GLS->theProgram, "offset");
    
    GLS->frustumScaleUnif = glGetUniformLocation(GLS->theProgram, "frustumScale");
    GLS->zNearUnif = glGetUniformLocation(GLS->theProgram, "zNear");
    GLS->zFarUnif = glGetUniformLocation(GLS->theProgram, "zFar");
    
    glUseProgram(GLS->theProgram);
    glUniform1f(GLS->frustumScaleUnif, 1.0f);
    glUniform1f(GLS->zNearUnif, 1.0f);
    glUniform1f(GLS->zFarUnif, 3.0f);
    glUseProgram(0);
    
    // NOTE(Eric): Init Vertex Buffer
    {
        glGenBuffers(1, &GLS->vertexBufferObject);
        
        glBindBuffer(GL_ARRAY_BUFFER, GLS->vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glGenVertexArrays(1, &GLS->vao);
        glBindVertexArray(GLS->vao);
    }
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    
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
    
    glUniform2f(GLS->offsetUniform, 0.5f, 0.5f);
    
    size_t colorData = sizeof(vertexData) / 2;
	glBindBuffer(GL_ARRAY_BUFFER, GLS->vertexBufferObject);
	glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)colorData);
    
	glDrawArrays(GL_TRIANGLES, 0, 36);
    
	glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
	glUseProgram(0);
    
    os->RefreshScreen();
}