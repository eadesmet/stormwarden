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
// ALSO not available from the gl_core_46.c files, generated from galogen
// Is it from some extension??? Maybe a problem with my machine's driver??
void (*GL_ClearDepthF)(float);


APP_PERMANENT_LOAD// NOTE(Eric): INIT
{
    os = os_;
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
    
    os->RefreshScreen();
}
