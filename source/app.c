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

// NOTE(Eric): This is a stupid hack. Somehow even though it's loading this procedure in
// LoadAllOpenGLProcedures(), it's still not resolving the external symbol?
// But when called manually, it works. I DON'T KNOW MAN.
void (*GL_ClearDepthF)(float);

APP_PERMANENT_LOAD// NOTE(Eric): INIT
{
    os = os_;
    GLS = &gls_;
    LoadAllOpenGLProcedures();
    GL_ClearDepthF = os->LoadOpenGLProcedure("glClearDepthf");
    
    // TODO(Eric): Clean this up and load/compile/createprogram in one step
    void* ShaderData = M_ArenaPush(&os->frame_arena, 2056);
    u64 ShaderLength = 0;
    String8 Path;
    Path.str = PATH_VS_4_MATRIX_PERSPECTIVE;
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
    GLS->perspectiveMatrixUnif = glGetUniformLocation(GLS->theProgram, "perspectiveMatrix");
    
    f32 fzNear = 0.5f;
    f32 fzFar = 3.0f;
    
    m4 m = {0};
    GLS->perspectiveMatrix = m;
    GLS->perspectiveMatrix.elements[0][0] = fFrustumScale;
    GLS->perspectiveMatrix.elements[1][1] = fFrustumScale;
    GLS->perspectiveMatrix.elements[2][2] = (fzFar + fzNear) / (fzNear - fzFar);
    GLS->perspectiveMatrix.elements[3][2] = (2 * fzFar * fzNear) / (fzNear - fzFar);
    GLS->perspectiveMatrix.elements[2][3] = -1.0f;
    
    glUseProgram(GLS->theProgram);
    glUniformMatrix4fv(GLS->perspectiveMatrixUnif, 1, GL_FALSE, &GLS->perspectiveMatrix.elements[0][0]);
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
}

APP_HOT_LOAD// NOTE(Eric): INIT AND ON CODE-RELOAD
{
    os = os_;
}

APP_HOT_UNLOAD {}

APP_UPDATE// NOTE(Eric): PER FRAME
{
    if (os->resized)
    {
        GLS->perspectiveMatrix.elements[0][0] = fFrustumScale / (os->window_size.width / (float)os->window_size.height);
        GLS->perspectiveMatrix.elements[1][1] = fFrustumScale;
        
        glUseProgram(GLS->theProgram);
        glUniformMatrix4fv(GLS->perspectiveMatrixUnif, 1, GL_FALSE, &GLS->perspectiveMatrix.elements[0][0]);
        glUseProgram(0);
        
        glViewport(0, 0, (GLsizei)os->window_size.width, (GLsizei)os->window_size.height);
        
        os->resized = 0;
    }
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    GL_ClearDepthF(1.0f);//glCearDepthf(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(GLS->theProgram);
    
    // NOTE(Eric): Now the two wedges are overlapping because we enabled GL_DEPTH_TEST
    // and set up the glClearDepth to 1, which is our glDepthRange zFar value.
    // Changing the first offset uniform z value to -0.75f causes slightly less overlap, etc.
    
    glBindVertexArray(GLS->vao);
    glUniform3f(GLS->offsetUniform, 0.0f, 0.0f, -1.0f);
    glDrawElements(GL_TRIANGLES, ArrayCount(indexData), GL_UNSIGNED_SHORT, 0);
    
    glUniform3f(GLS->offsetUniform, 0.0f, 0.0f, -1.0f);
    glDrawElementsBaseVertex(GL_TRIANGLES, ArrayCount(indexData), GL_UNSIGNED_SHORT, 0, numberOfVertices/2);
    
    glBindVertexArray(0);
    glUseProgram(0);
    
    os->RefreshScreen();
}