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


typedef struct render_info render_info;
struct render_info
{
    ID3D11InputLayout *InputLayout;
    ID3D11Buffer *VertexBuffer;
    ID3D11Buffer *IndexBuffer;
    ID3D11VertexShader *VertexShader;
    ID3D11PixelShader *PixelShader;
    ID3D11Buffer *ConstantBuffer;
};

enum render_info_type
{
    RenderInfoType_Square,
    
    RenderInfoType_Count
};

typedef struct entity_square entity_square;
struct entity_square
{
    vertex_data VertexData[4];
};

typedef struct game_state game_state;
struct game_state
{
    render_info RenderInfos[RenderInfoType_Count];
    
    u32 SquareCount;
    entity_square Squares[64];
};

#endif //ENTITY_H
