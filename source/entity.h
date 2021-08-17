/* date = July 19th 2021 4:18 pm */

#ifndef ENTITY_H
#define ENTITY_H

typedef struct entity entity;
struct entity
{
    v2 p;
};

typedef struct vertex_data vertex_data;
struct vertex_data
{
    v2 Position;
    v2 UV;
    
    v3 Color;
};


#endif //ENTITY_H
