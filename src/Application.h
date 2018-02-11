#include <SDL.h>

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
    bool m_IsRunning;
    SDL_Window* m_Window;
};
