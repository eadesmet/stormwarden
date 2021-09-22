

#include "entity.h"

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