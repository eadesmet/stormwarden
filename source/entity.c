

#include "entity.h"

#if 0
internal void
CreateSquare(game_state *GameState)
{
    entity_square NewSquare = {0};
    
    // NOTE(Eric): This is getting set in the VertexBuffer of Square,
    // so having it here doesn't really make sense.
    NewSquare.VertexData = 
    {
        { { -0.75f, +0.50f }, { 25.0f, 50.0f }, { 1, 0, 0 } }, // Top-left
        { { +0.75f, -0.50f }, {  0.0f,  0.0f }, { 0, 1, 0 } }, // Bottom-right
        { { -0.75f, -0.50f }, { 50.0f,  0.0f }, { 0, 0, 1 } }, // Bottom-left
        { { +0.75f, +0.50f }, { 50.0f,  0.0f }, { 0, 0, 1 } }, // Top-right
    };
    
    GameState->Squares[GameState->SquareCount++] = NewSquare;
}
#endif

internal game_camera
CreateCamera(f32 Fov, f32 AspectRatio)
{
    game_camera Result = {0};
    
    Result.Position = v3(0.0f, 0.0f, 2.0f);
    
    // NOTE(Eric): What could M4LookAt() do for me here?
    Result.LookAt = v3(0.0f, 0.0f, -1.0f);
    
    Result.Perspective = M4Perspective(Fov, AspectRatio, 0.1f, 1000.0f);
    
    return Result;
}