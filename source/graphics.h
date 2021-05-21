
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
    GLuint offsetUniform;
    GLuint perspectiveMatrixUnif;
    
    m4 perspectiveMatrix;
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

global GL_State gls_;
global GL_State* GLS; 

// NOTE(Eric): Changing this value is interesting. smaller = smaller objects, negative = image flipped
global f32 fFrustumScale = 1.0f;

const int numberOfVertices = 36;

#define RIGHT_EXTENT 0.8f
#define LEFT_EXTENT -RIGHT_EXTENT
#define TOP_EXTENT 0.20f
#define MIDDLE_EXTENT 0.0f
#define BOTTOM_EXTENT -TOP_EXTENT
#define FRONT_EXTENT -1.25f
#define REAR_EXTENT -1.75f

#define GREEN_COLOR 0.75f, 0.75f, 1.0f, 1.0f
#define BLUE_COLOR 	0.0f, 0.5f, 0.0f, 1.0f
#define RED_COLOR 1.0f, 0.0f, 0.0f, 1.0f
#define GREY_COLOR 0.8f, 0.8f, 0.8f, 1.0f
#define BROWN_COLOR 0.5f, 0.5f, 0.0f, 1.0f

const float vertexData[] = {
	//Object 1 positions
	LEFT_EXTENT,	TOP_EXTENT,		REAR_EXTENT,
	LEFT_EXTENT,	MIDDLE_EXTENT,	FRONT_EXTENT,
	RIGHT_EXTENT,	MIDDLE_EXTENT,	FRONT_EXTENT,
	RIGHT_EXTENT,	TOP_EXTENT,		REAR_EXTENT,
    
	LEFT_EXTENT,	BOTTOM_EXTENT,	REAR_EXTENT,
	LEFT_EXTENT,	MIDDLE_EXTENT,	FRONT_EXTENT,
	RIGHT_EXTENT,	MIDDLE_EXTENT,	FRONT_EXTENT,
	RIGHT_EXTENT,	BOTTOM_EXTENT,	REAR_EXTENT,
    
	LEFT_EXTENT,	TOP_EXTENT,		REAR_EXTENT,
	LEFT_EXTENT,	MIDDLE_EXTENT,	FRONT_EXTENT,
	LEFT_EXTENT,	BOTTOM_EXTENT,	REAR_EXTENT,
    
	RIGHT_EXTENT,	TOP_EXTENT,		REAR_EXTENT,
	RIGHT_EXTENT,	MIDDLE_EXTENT,	FRONT_EXTENT,
	RIGHT_EXTENT,	BOTTOM_EXTENT,	REAR_EXTENT,
    
	LEFT_EXTENT,	BOTTOM_EXTENT,	REAR_EXTENT,
	LEFT_EXTENT,	TOP_EXTENT,		REAR_EXTENT,
	RIGHT_EXTENT,	TOP_EXTENT,		REAR_EXTENT,
	RIGHT_EXTENT,	BOTTOM_EXTENT,	REAR_EXTENT,
    
	//Object 2 positions
	TOP_EXTENT,		RIGHT_EXTENT,	REAR_EXTENT,
	MIDDLE_EXTENT,	RIGHT_EXTENT,	FRONT_EXTENT,
	MIDDLE_EXTENT,	LEFT_EXTENT,	FRONT_EXTENT,
	TOP_EXTENT,		LEFT_EXTENT,	REAR_EXTENT,
    
	BOTTOM_EXTENT,	RIGHT_EXTENT,	REAR_EXTENT,
	MIDDLE_EXTENT,	RIGHT_EXTENT,	FRONT_EXTENT,
	MIDDLE_EXTENT,	LEFT_EXTENT,	FRONT_EXTENT,
	BOTTOM_EXTENT,	LEFT_EXTENT,	REAR_EXTENT,
    
	TOP_EXTENT,		RIGHT_EXTENT,	REAR_EXTENT,
	MIDDLE_EXTENT,	RIGHT_EXTENT,	FRONT_EXTENT,
	BOTTOM_EXTENT,	RIGHT_EXTENT,	REAR_EXTENT,
    
	TOP_EXTENT,		LEFT_EXTENT,	REAR_EXTENT,
	MIDDLE_EXTENT,	LEFT_EXTENT,	FRONT_EXTENT,
	BOTTOM_EXTENT,	LEFT_EXTENT,	REAR_EXTENT,
    
	BOTTOM_EXTENT,	RIGHT_EXTENT,	REAR_EXTENT,
	TOP_EXTENT,		RIGHT_EXTENT,	REAR_EXTENT,
	TOP_EXTENT,		LEFT_EXTENT,	REAR_EXTENT,
	BOTTOM_EXTENT,	LEFT_EXTENT,	REAR_EXTENT,
    
	//Object 1 colors
	GREEN_COLOR,
	GREEN_COLOR,
	GREEN_COLOR,
	GREEN_COLOR,
    
	BLUE_COLOR,
	BLUE_COLOR,
	BLUE_COLOR,
	BLUE_COLOR,
    
	RED_COLOR,
	RED_COLOR,
	RED_COLOR,
    
	GREY_COLOR,
	GREY_COLOR,
	GREY_COLOR,
    
	BROWN_COLOR,
	BROWN_COLOR,
	BROWN_COLOR,
	BROWN_COLOR,
    
	//Object 2 colors
	RED_COLOR,
	RED_COLOR,
	RED_COLOR,
	RED_COLOR,
    
	BROWN_COLOR,
	BROWN_COLOR,
	BROWN_COLOR,
	BROWN_COLOR,
    
	BLUE_COLOR,
	BLUE_COLOR,
	BLUE_COLOR,
    
	GREEN_COLOR,
	GREEN_COLOR,
	GREEN_COLOR,
    
	GREY_COLOR,
	GREY_COLOR,
	GREY_COLOR,
	GREY_COLOR,
};

const GLshort indexData[] =
{
	0, 2, 1,
	3, 2, 0,
    
	4, 5, 6,
	6, 7, 4,
    
	8, 9, 10,
	11, 13, 12,
    
	14, 16, 15,
	17, 16, 14,
};
