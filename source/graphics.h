
typedef struct GL_State GL_State;
struct GL_State
{
    GLuint vShader;
    GLuint fShader;
    GLuint theProgram;
    GLuint vertexBufferObject;
    GLuint indexBufferObject;
    GLuint vao;
    GLuint vao2;
    
    GLuint ModelToCameraMatrixUnif;
    GLuint CameraToClipMatrixUnif;
    
    m4 CameraToClipMatrix;
};


global char* PATH_VS_1 = "C:/_Eric/Code/d3d/stormwarden/source/shaders/SimpleVS.vert";
global char* PATH_FS_1 = "C:/_Eric/Code/d3d/stormwarden/source/shaders/PositionFS.frag";

global char* PATH_VS_2 = "C:/_Eric/Code/d3d/stormwarden/source/shaders/2_PositionVS.vert";
global char* PATH_FS_2 = "C:/_Eric/Code/d3d/stormwarden/source/shaders/2_PositionFS.frag";

global char* PATH_VS_3 = "C:/_Eric/Code/d3d/stormwarden/source/shaders/3_PositionOffset.vert";
global char* PATH_VS_3_CALC = "C:/_Eric/Code/d3d/stormwarden/source/shaders/3_CalcOffset.vert";
global char* PATH_FS_3_CALC = "C:/_Eric/Code/d3d/stormwarden/source/shaders/3_CalcColor.frag";

global char* PATH_VS_4_ORTHO_CUBE = "C:/_Eric/Code/d3d/stormwarden/source/shaders/4_OrthoCube.vert";
global char* PATH_VS_4_MANUAL_PERSPECTIVE = "C:/_Eric/Code/d3d/stormwarden/source/shaders/4_ManualPerspective.vert";
global char* PATH_VS_4_MATRIX_PERSPECTIVE = "C:/_Eric/Code/d3d/stormwarden/source/shaders/4_MatrixPerspective.vert";
global char* PATH_FS_4_ORTHO_CUBE = "C:/_Eric/Code/d3d/stormwarden/source/shaders/4_OrthoCube.frag";


global char* PATH_VS_6 = "C:/_Eric/Code/d3d/stormwarden/source/shaders/6_PosColorLocalTransform.vert";
global char* PATH_FS_6 = "C:/_Eric/Code/d3d/stormwarden/source/shaders/6_ColorPassthrough.frag";


global GL_State gls_;
global GL_State* GLS; 

// NOTE(Eric): Changing this value is interesting. smaller = smaller objects, negative = image flipped
global f32 fFrustumScale = 1.0f;

const int numberOfVertices = 8;

#define GREEN_COLOR 0.0f, 1.0f, 0.0f, 1.0f
#define BLUE_COLOR  0.0f, 0.0f, 1.0f, 1.0f
#define RED_COLOR   1.0f, 0.0f, 0.0f, 1.0f
#define GREY_COLOR  0.8f, 0.8f, 0.8f, 1.0f
#define BROWN_COLOR 0.5f, 0.5f, 0.0f, 1.0f

const float vertexData[] =
{
	+1.0f, +1.0f, +1.0f,
	-1.0f, -1.0f, +1.0f,
	-1.0f, +1.0f, -1.0f,
	+1.0f, -1.0f, -1.0f,
    
	-1.0f, -1.0f, -1.0f,
	+1.0f, +1.0f, -1.0f,
	+1.0f, -1.0f, +1.0f,
	-1.0f, +1.0f, +1.0f,
    
	GREEN_COLOR,
	BLUE_COLOR,
	RED_COLOR,
	BROWN_COLOR,
    
	GREEN_COLOR,
	BLUE_COLOR,
	RED_COLOR,
	BROWN_COLOR,
};

const GLshort indexData[] =
{
	0, 1, 2,
	1, 0, 3,
	2, 3, 0,
	3, 2, 1,
    
	5, 4, 6,
	4, 5, 7,
	7, 6, 4,
	6, 7, 5,
};
