/* date = September 30th 2021 4:15 pm */

#ifndef RENDER_H
#define RENDER_H

typedef struct square_constant square_constant;
struct square_constant
{
    v4 Pos;
    v4 Size;
    v4 Color;
    
    m4 Projection;
};


#endif //RENDER_H
