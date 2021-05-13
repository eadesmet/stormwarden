#include "language_layer.h"
#include "maths.h"
#include "memory.h"
#include "strings.h"
#include "perlin.h"
#include "os.h"
#include "opengl.h"

#include "language_layer.c"
#include "maths.c"
#include "memory.c"
#include "strings.c"
#include "perlin.c"
#include "os.c"

// NOTE(Eric): INIT
APP_PERMANENT_LOAD
{
    os = os_;
    LoadAllOpenGLProcedures();
    
    
}

// NOTE(Eric): INIT AND ON CODE-RELOAD
APP_HOT_LOAD
{
    os = os_;
}

APP_HOT_UNLOAD {}

// NOTE(Eric): PER FRAME
APP_UPDATE
{
    glClearColor(1, 0, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    os->RefreshScreen();
}