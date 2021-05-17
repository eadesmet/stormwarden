
typedef struct GL_State GL_State;
struct GL_State
{
    GLuint vShader;
    GLuint fShader;
    GLuint theProgram;
    GLuint positionBufferObject;
    GLuint vao;
    
};


const float vertexPositions[] = {
	0.75f, 0.75f, 0.0f, 1.0f,
	0.75f, -0.75f, 0.0f, 1.0f,
	-0.75f, -0.75f, 0.0f, 1.0f,
    
    -0.75f, -0.70f, 0.0f, 1.0f,
	-0.75f, 0.80f, 0.0f, 1.0f,
	0.75f, 0.80f, 0.0f, 1.0f,
};

global char* PATH_VS_SIMPLE = "C:/_Eric/Code/d3d/stormwarden/source/shaders/SimpleVS.vert";
global char* PATH_FS_POSITION = "C:/_Eric/Code/d3d/stormwarden/source/shaders/PositionFS.frag";

//global char* PATH_FS_MULTI_INPUT = "C:/_Eric/Code/d3d/stormwarden/source/shaders/PositionFS.frag";

global GL_State _gls;
global GL_State* GLS; 