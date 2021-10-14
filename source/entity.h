/* date = July 19th 2021 4:18 pm */

#ifndef ENTITY_H
#define ENTITY_H

typedef struct vertex_data vertex_data;
struct vertex_data
{
    v2 Position;
    v2 UV;
    
    v3 Color;
};

typedef struct game_camera game_camera;
struct game_camera
{
    v3 Position;
    m4 LookAt;
    
    m4 Perspective;
};

typedef struct entity_square entity_square;
struct entity_square
{
    vertex_data VertexData[4];
};

typedef struct game_state game_state;
struct game_state
{
    game_camera Camera;
    
    render_info RenderInfos[RenderInfoType_Count];
    
    u32 SquareCount;
    entity_square Squares[64];
};

#endif //ENTITY_H
