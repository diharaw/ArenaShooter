#include "RenderDevice.h"
#include <SDL.h>
#include <SDL_opengl.h>

#define TE_DECLARE_MAIN()        \
int main(int argc, char* argv[]) \
{                                \
    Application app;             \
    return app.Run(argc, argv);  \
}

class Application
{
public:
    Application();
    ~Application();
    bool Init();
    void Shutdown();
    int Run(int argc, char* argv[]);
    void Frame();
    
private:
    void EventLoop();
    
private:
	RenderDevice m_device;
	int m_width;
	int m_height;
    bool m_IsRunning;
	SDL_GLContext m_context;
    SDL_Window* m_Window;
};
