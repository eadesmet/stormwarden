
typedef struct GL_State GL_State;
struct GL_State
{
    GLuint vShader;
    GLuint fShader;
    GLuint theProgram;
    GLuint vertexBufferObject;
    GLuint vao;
    
};

const float vertexData[] = {
    0.0f,    0.5f, 0.0f, 1.0f,
    0.5f, -0.366f, 0.0f, 1.0f,
    -0.5f, -0.366f, 0.0f, 1.0f,
    1.0f,    0.0f, 0.0f, 1.0f,
    0.0f,    1.0f, 0.0f, 1.0f,
    0.0f,    0.0f, 1.0f, 1.0f,
};

global char* PATH_VS_1 = "C:/_Eric/Code/d3d/stormwarden/source/shaders/SimpleVS.vert";
global char* PATH_FS_1 = "C:/_Eric/Code/d3d/stormwarden/source/shaders/PositionFS.frag";

global char* PATH_VS_2 = "C:/_Eric/Code/d3d/stormwarden/source/shaders/2_PositionVS.vert";
global char* PATH_FS_2 = "C:/_Eric/Code/d3d/stormwarden/source/shaders/2_PositionFS.frag";


global GL_State gls_;
global GL_State* GLS; 