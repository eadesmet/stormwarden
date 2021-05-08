
#ifndef MATH_H
#define MATH_H

#define _USE_MATH_DEFINES
#include <math.h>


// NOTE(Eric): Remember! Code something FIRST, then compress and pull things out.


struct float2
{
    float x, y;
};

struct float4
{
    float x, y, z, w;
};




struct v3
{
    FLOAT X;
    FLOAT Y;
    FLOAT Z;
};

struct v4
{
    FLOAT R;
    FLOAT G;
    FLOAT B;
    FLOAT A;
};

struct VERTEX
{
    v3 P;
    v4 Color;
};


#endif //MATH_H
