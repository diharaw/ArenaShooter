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

InputLayout* m_il;
Shader* m_vs;
Shader* m_fs;
ShaderProgram* m_program;
RasterizerState* m_rs;
DepthStencilState* m_ds;
IndexBuffer* m_IBO;
VertexBuffer* m_VBO;
VertexArray* m_VAO;

const char* kVS = R"(layout (location = 0) in vec3 VS_IN_Position;

void main()
{
    gl_Position = vec4(VS_IN_Position, 1.0);
})";

const char* kFS = R"(out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f);
})";

float kVerts[9] = {
	0.0f, 0.8f, 0.0f,
	-0.8f, -0.8f, 0.0f,
	0.8f, -0.8f, 0.0f,
};

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

	m_vs = m_device.create_shader(kVS, ShaderType::VERTEX);
	m_fs = m_device.create_shader(kFS, ShaderType::FRAGMENT);

	if (!m_vs || !m_fs)
	{
		return false;
}

	Shader* shaders[] = { m_vs, m_fs };
	m_program = m_device.create_shader_program(shaders, 2);

	if (!m_program)
	{
		return false;
	}
    
	RasterizerStateCreateDesc rs_desc;
	memset(&rs_desc, 0, sizeof(RasterizerStateCreateDesc));
	rs_desc.cull_mode = CullMode::NONE;
	rs_desc.fill_mode = FillMode::SOLID;
	rs_desc.front_winding_ccw = true;
	rs_desc.multisample = true;
	rs_desc.scissor = false;

	m_rs = m_device.create_rasterizer_state(rs_desc);

	DepthStencilStateCreateDesc ds_desc;
	memset(&ds_desc, 0, sizeof(DepthStencilStateCreateDesc));
	ds_desc.depth_mask = false;
	ds_desc.enable_depth_test = false;
	ds_desc.enable_stencil_test = false;
	ds_desc.depth_cmp_func = ComparisonFunction::LESS_EQUAL;

	m_ds = m_device.create_depth_stencil_state(ds_desc);

	BufferCreateDesc bc;
	InputLayoutCreateDesc ilcd;
	VertexArrayCreateDesc vcd;

	memset(&bc, 0, sizeof(BufferCreateDesc));
	bc.data = (float*)&kVerts[0];
	bc.data_type = DataType::FLOAT;
	bc.size = sizeof(float) * 9;
	bc.usage_type = BufferUsageType::STATIC;

	m_VBO = m_device.create_vertex_buffer(bc);

	/*memset(&bc, 0, sizeof(BufferCreateDesc));
	bc.data = (float*)&indices[0];
	bc.data_type = DataType::UINT32;
	bc.size = sizeof(uint32_t)* indices.size();
	bc.usage_type = BufferUsageType::STATIC;

	m_IBO = m_device.create_index_buffer(bc);*/

	InputElement elements[] =
	{
		{ 3, DataType::FLOAT, false, 0, "POSITION" }
	};

	memset(&ilcd, 0, sizeof(InputLayoutCreateDesc));
	ilcd.elements = elements;
	ilcd.num_elements = 1;
	ilcd.vertex_size = sizeof(float) * 3;

	m_il = m_device.create_input_layout(ilcd);

	memset(&vcd, 0, sizeof(VertexArrayCreateDesc));
	vcd.index_buffer = nullptr;
	vcd.vertex_buffer = m_VBO;
	vcd.layout = m_il;

	m_VAO = m_device.create_vertex_array(vcd);

	if (!m_VAO || !m_VBO )
	{
		return false;
	}

	SDL_GL_GetDrawableSize(m_Window, &m_width, &m_height);

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

	m_device.bind_framebuffer(nullptr);
	m_device.set_viewport(m_width, m_height, 0, 0);

	float clear[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_device.clear_framebuffer(ClearTarget::ALL, clear);

	//m_device.bind_rasterizer_state(m_rs);
	//m_device.bind_depth_stencil_state(m_ds);
	m_device.bind_shader_program(m_program);
	m_device.bind_vertex_array(m_VAO);

	m_device.set_primitive_type(PrimitiveType::TRIANGLES);
	m_device.draw(0, 3);

	SDL_GL_SwapWindow(m_Window);
}

bool Application::Init()
{
    Uint32 flags = SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_HAPTIC | SDL_INIT_JOYSTICK;
    
	if (SDL_Init(flags) != 0)
		return false;

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
  
    m_Window = SDL_CreateWindow("Application",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               1280,
                               720,
								SDL_WINDOW_OPENGL);
    if (!m_Window)
        return false;
    
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

	m_context = SDL_GL_CreateContext(m_Window);
	SDL_GL_MakeCurrent(m_Window, m_context);

    m_IsRunning = true;
    return m_device.init();
}

void Application::Shutdown()
{
	SDL_GL_DeleteContext(m_context);
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
