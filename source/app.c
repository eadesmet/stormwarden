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

// NOTE(Eric): Getting into DirectX now instead, going through the Luna book
//#include "gl_arcsynth.h"
//#include "gl_arcsynth.c"


APP_PERMANENT_LOAD// NOTE(Eric): INIT
{
    os = os_;
    
}

APP_HOT_LOAD// NOTE(Eric): INIT AND ON CODE-RELOAD
{
    os = os_;
}

APP_HOT_UNLOAD {}

APP_UPDATE// NOTE(Eric): PER FRAME
{
    if (os->resized)
    {
        // TODO(Eric): RESET VIEWPORT
        //glViewport(0, 0, (GLsizei)os->window_size.width, (GLsizei)os->window_size.height);
        
        os->resized = 0;
    }
    
    local_persist b32 DepthClampingActive = 0;
    if (os->event_count > 0)
    {
        // NOTE(Eric): Not sure if this is how I'm supposed to handle events in the app,
        // but it's working for now.
        for (u32 EventIndex = 0;
             EventIndex < os->event_count;
             EventIndex++)
        {
            OS_Event *Event = 0;
            OS_GetNextEvent(&Event);
            if(!Event) break;
            
            if (Event->type == OS_EventType_KeyPress)
            {
                switch(Event->key)
                {
                    case Key_Space:
                    {
                        
                    }break;
                    case Key_Esc:
                    {
                        os->quit = 1;
                    }break;
                    default:break;
                }
            }
            
            OS_EatEvent(Event);
        }
    }
    
    os->RefreshScreen();
}
