#include "Application.h"
#include "Platform.h"

#include <stdio.h>

#if defined(TE_PLATFORM_EMSCRIPTEN)
#include <emscripten/emscripten.h>
#endif

#if defined(TE_PLATFORM_EMSCRIPTEN) || defined(TE_PLATFORM_IOS)

void ApplicationFrame(void* app)
{
    ((Application*)app)->Frame();
}

#endif

#if defined(TE_PLATFORM_IOS)

int HandleAppEvents(void* app, SDL_Event* event)
{
    switch (event->type)
    {
        case SDL_APP_TERMINATING:
            ((Application*)app)->Shutdown();
            return 0;
        case SDL_APP_LOWMEMORY:
            return 0;
        case SDL_APP_WILLENTERBACKGROUND:
            return 0;
        case SDL_APP_DIDENTERBACKGROUND:
            return 0;
        case SDL_APP_WILLENTERFOREGROUND:
            return 0;
        case SDL_APP_DIDENTERFOREGROUND:
            return 0;
        default:
            return 1;
    }
}

#endif

Application::Application() : m_IsRunning(false),
                             m_Window(nullptr)
{
    printf("Initializing..\n");
}

Application::~Application()
{
    
}

int Application::Run(int argc, char* argv[])
{
    if(!Init())
        return 1;
    
#if defined(TE_PLATFORM_EMSCRIPTEN)
    emscripten_set_main_loop_arg(ApplicationFrame, this, 0, 1);
#elif defined(TE_PLATFORM_IPHONE)
    SDL_iPhoneSetEventPump(true);
    SDL_SetEventFilter(HandleAppEvents, this);
    SDL_iPhoneSetAnimationCallback(m_Window, 1, &ApplicationFrame, this);
#else
    while (m_IsRunning)
    {
        Frame();
    }
    
    Shutdown();
#endif
    
    return 0;
}

void Application::Frame()
{
    EventLoop();
}

bool Application::Init()
{
    Uint32 flags = SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_HAPTIC | SDL_INIT_JOYSTICK;
    
    if (SDL_Init(flags) != 0)
        return false;
    
    m_Window = SDL_CreateWindow("Application",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               800,
                               600,
                               0);
    if (!m_Window)
        return false;
    
    m_IsRunning = true;
    return true;
}

void Application::Shutdown()
{
    SDL_DestroyWindow(m_Window);
}

void Application::EventLoop()
{
    SDL_Event event;
    
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_MOUSEWHEEL:
                break;
                
            case SDL_MOUSEMOTION:
            {
                SDL_bool relative = SDL_GetRelativeMouseMode();
                break;
            }
                
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
            {

                break;
            }
                
            case SDL_KEYUP:
            case SDL_KEYDOWN:
            {
                if(event.key.repeat == 0)
                {

                }
                break;
            }
                
            case SDL_QUIT:
                m_IsRunning = false;
                break;
                
            default:
                break;
        }
    }
}

TE_DECLARE_MAIN();
