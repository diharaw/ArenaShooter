#include "RenderDevice.h"
#include "glad.h"
#include "utility.h"
#include "logger.h"

#define GFX_ENABLE_ERROR_CHECK

#ifdef GFX_ENABLE_ERROR_CHECK
#define GL_CHECK_ERROR(x)																		  \
x; {                                                                                              \
GLenum err(glGetError());																		  \
																								  \
while (err != GL_NO_ERROR)																		  \
{																								  \
std::string error;																				  \
																								  \
switch (err)																					  \
{																								  \
case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;						  \
case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;						  \
case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;						  \
case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;						  \
case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;		  \
}																								  \
																								  \
std::string formatted_error = "OPENGL ERROR : ";                                                  \
formatted_error += error;                                                                         \
std::cout <<  "LINE:" << __LINE__ <<", FILE:" << __FILE__ << formatted_error << std::endl;        \
err = glGetError();																				  \
}																								  \
}
#else
#define GL_CHECK_ERROR(x)	x
#endif

const GLenum kTextureFormatTable[][3] =
{
	{ GL_RGB32F, GL_RGB, GL_FLOAT } ,
	{ GL_RGBA32F, GL_RGBA, GL_FLOAT } ,
	{ GL_RGB32UI, GL_RGB, GL_UNSIGNED_INT } ,
	{ GL_RGBA32UI, GL_RGBA, GL_UNSIGNED_INT } ,
	{ GL_RGB32I, GL_RGB, GL_INT } ,
	{ GL_RGBA32I, GL_RGBA, GL_INT } ,
	{ GL_RGB16F, GL_RGB, GL_FLOAT } ,
    { GL_RGBA16F, GL_RGBA, GL_FLOAT } ,
    { GL_RGB16UI, GL_RGB, GL_UNSIGNED_INT } ,
	{ GL_RGBA16UI, GL_RGBA, GL_UNSIGNED_INT } ,
    { GL_RGB16I, GL_RGB, GL_INT } ,
	{ GL_RGBA16I, GL_RGBA, GL_INT } ,
    { GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE } ,
	{ GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE } ,
    { GL_SRGB8, GL_RGB, GL_UNSIGNED_BYTE } ,
    { GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE } ,
    { GL_RGB8_SNORM, GL_RGB, GL_BYTE } ,
    { GL_RGBA8_SNORM, GL_RGBA, GL_BYTE } ,
    { GL_RGB8I, GL_RGB, GL_INT } ,
    { GL_RGBA8I, GL_RGBA, GL_INT } ,
    { GL_RGB8I, GL_RGB, GL_UNSIGNED_INT } ,
	{ GL_RGBA8UI, GL_RGBA, GL_UNSIGNED_INT } ,
	{ GL_R8, GL_RED, GL_UNSIGNED_BYTE } ,
	{ GL_R8_SNORM, GL_RED, GL_BYTE } ,
	{ GL_DEPTH_STENCIL, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV } ,
	{ GL_DEPTH_STENCIL, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8 } ,
	{ GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT }
};

const GLenum kShaderTypeTable[] =
{
	GL_VERTEX_SHADER,
	GL_FRAGMENT_SHADER,
	GL_GEOMETRY_SHADER,
	GL_TESS_CONTROL_SHADER,
	GL_TESS_EVALUATION_SHADER,
	GL_COMPUTE_SHADER
};

const GLenum kBufferUsageTable[] =
{
	GL_STATIC_DRAW,
	GL_DYNAMIC_DRAW,
	GL_STREAM_DRAW
};

const GLenum kMapUsageTable[] =
{
	GL_READ_ONLY,
	GL_WRITE_ONLY,
	GL_READ_WRITE
};

const GLenum kBufferDataTypeTable[] =
{
	GL_BYTE,
	GL_UNSIGNED_BYTE,
	GL_SHORT,
	GL_INT,
	GL_UNSIGNED_SHORT,
	GL_UNSIGNED_INT,
	GL_FLOAT
};

const GLenum kCullModeTable[] =
{
	GL_FRONT,
	GL_BACK,
	GL_FRONT_AND_BACK
};

const GLenum kFillModeTable[] =
{
	GL_FILL,
	GL_LINE
};

const GLenum kComparisonFunctionTable[] =
{
	GL_NEVER,
	GL_LESS,
	GL_EQUAL,
	GL_LEQUAL,
	GL_GREATER,
	GL_NOTEQUAL,
	GL_GEQUAL,
	GL_ALWAYS
};

const GLenum kStencilOperationTable[] =
{
	GL_KEEP,
	GL_ZERO,
	GL_REPLACE,
	GL_INCR,
	GL_DECR,
	GL_INVERT,
	GL_INCR_WRAP,
	GL_DECR_WRAP
};

const GLenum kTextureWrapModeTable[] =
{
	GL_REPEAT,
	GL_MIRRORED_REPEAT,
	GL_CLAMP_TO_EDGE,
	GL_CLAMP_TO_BORDER
};

const GLenum kTextureMagFilteringModeTable[] =
{
    GL_LINEAR,
    GL_NEAREST,
    GL_LINEAR,
    GL_NEAREST,
    GL_TEXTURE_MAX_ANISOTROPY_EXT,
    GL_LINEAR,
    GL_NEAREST
};

const GLenum kTextureMinFilteringModeTable[] =
{
    GL_LINEAR,
    GL_NEAREST,
	GL_LINEAR_MIPMAP_LINEAR,
	GL_NEAREST_MIPMAP_NEAREST,
	GL_TEXTURE_MAX_ANISOTROPY_EXT,
	GL_LINEAR_MIPMAP_NEAREST,
	GL_NEAREST_MIPMAP_LINEAR
};

const GLenum kDrawPrimitiveTypeTable[] =
{
	GL_POINTS,
	GL_TRIANGLES,
	GL_TRIANGLE_STRIP,
	GL_LINES,
	GL_LINE_STRIP
};

RenderDevice::RenderDevice()
{
    
}

RenderDevice::~RenderDevice()
{
    
}

bool RenderDevice::init()
{
	if (!gladLoadGL())
	{
		std::cout << "Failed to load functions" << std::endl;
		return false;
	}
	else
	{
        glEnable(GL_MULTISAMPLE);
		return true;
	}
}

Shader* RenderDevice::create_shader(const char* source, uint32_t type)
{
	Shader* shader = new Shader();
	shader->type = type;

	GL_CHECK_ERROR(shader->id = glCreateShader(kShaderTypeTable[type]));

	GLint success;
	GLchar infoLog[512];

#if defined(__APPLE__)
    shader->source = "#version 410 core\n" + std::string(source);
#else
    shader->source = "#version 430 core\n" + std::string(source);
#endif

	const GLchar* src = shader->source.c_str();

	GL_CHECK_ERROR(glShaderSource(shader->id, 1, &src, NULL));
	GL_CHECK_ERROR(glCompileShader(shader->id));
	GL_CHECK_ERROR(glGetShaderiv(shader->id, GL_COMPILE_STATUS, &success));

	if (success == GL_FALSE)
	{
		glGetShaderInfoLog(shader->id, 512, NULL, infoLog);

		std::string log_error = "Shader compilation failed";
		log_error += std::string(infoLog);

		std::cout << log_error << std::endl;

		return nullptr;
	}

	std::cout << "Shader successfully compiled." << std::endl;

	return shader;
}

ShaderProgram* RenderDevice::create_shader_program(Shader** shaders, uint32_t count)
{
	ShaderProgram* shaderProgram = new ShaderProgram();
	GL_CHECK_ERROR(shaderProgram->id = glCreateProgram());

	for (uint32_t i = 0; i < count; i++)
	{
		if (count > 1 && shaders[i]->type == ShaderType::COMPUTE)
        {
            delete shaderProgram;
            return nullptr;
        }

		GL_CHECK_ERROR(glAttachShader(shaderProgram->id, shaders[i]->id));
        shaderProgram->shader_map[shaders[i]->type] = shaders[i];
	}

	GL_CHECK_ERROR(glLinkProgram(shaderProgram->id));

	GLint success;
	char infoLog[512];

	glGetProgramiv(shaderProgram->id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram->id, 512, NULL, infoLog);

		std::string log_error = "Shader program linking failed";
		log_error += std::string(infoLog);

        LOG_ERROR(log_error);

		return nullptr;
	}

    // Bind Uniform Buffers
    for (auto it : shaderProgram->shader_map)
    {
        StringList uboList = Utility::find_line("#binding", it.second->source);

        for (int i = 0; i < uboList.size(); i++)
        {
            StringList tokens = Utility::delimit(" ", uboList[i]);
            std::string uniformName = tokens[3];
            GLuint binding = (GLuint)atoi(tokens[5].c_str());
            const GLchar* uniformNameChar = uniformName.c_str();
            GL_CHECK_ERROR(GLuint uboIndex = glGetUniformBlockIndex(shaderProgram->id, uniformNameChar));

            if (uboIndex == GL_INVALID_INDEX)
            {
                String uniform_error = "Failed to get Uniform Block Index for Uniform Buffer : ";
                uniform_error += uniformName;

                LOG_ERROR(uniform_error);
            }
            else
                GL_CHECK_ERROR(glUniformBlockBinding(shaderProgram->id, uboIndex, binding));
        }
    }

    // Bind Uniform Samplers
    for (auto it : shaderProgram->shader_map)
    {
        StringList samplerList = Utility::find_line("uniform sampler", it.second->source);

        for (int i = 0; i < samplerList.size(); i++)
        {
            StringList tokens = Utility::delimit(" ", samplerList[i]);
            std::string uniformName = tokens[2];
            uniformName.erase(uniformName.end() - 1, uniformName.end());

            GLuint binding = (GLuint)atoi(tokens[4].c_str());
            const GLchar* uniformNameChar = uniformName.c_str();
            GL_CHECK_ERROR(GLuint location = glGetUniformLocation(shaderProgram->id, uniformNameChar));

            if (location == GL_INVALID_INDEX)
            {
                String uniform_error = "Failed to get Uniform Location for Uniform : ";
                uniform_error += uniformName;

                LOG_ERROR(uniform_error);
            }

            it.second->sampler_bindings[binding] = location;
        }
    }

	return shaderProgram;
}

void RenderDevice::attach_render_target(Framebuffer* framebuffer, Texture* render_target)
{
	framebuffer->render_targets[framebuffer->num_render_targets++] = render_target;

	GL_CHECK_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id));
	GL_CHECK_ERROR(glBindTexture(render_target->gl_texture_target, framebuffer->id));
	GL_CHECK_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (framebuffer->num_render_targets - 1),
		render_target->gl_texture_target,
		render_target->id, 0));

	std::vector<GLuint> drawBuffers;

	for (int i = 0; i < framebuffer->num_render_targets; i++)
		drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);

	GL_CHECK_ERROR(glDrawBuffers(framebuffer->num_render_targets, &drawBuffers[0]));

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    
	GL_CHECK_ERROR(glBindTexture(render_target->gl_texture_target, 0));
	GL_CHECK_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void RenderDevice::attach_depth_stencil_target(Framebuffer* framebuffer, Texture* render_target)
{
	framebuffer->depth_target = render_target;
	GL_CHECK_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id));
	GL_CHECK_ERROR(glBindTexture(render_target->gl_texture_target, framebuffer->id));
	GL_CHECK_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, render_target->gl_texture_target, render_target->id, 0));
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    
	GL_CHECK_ERROR(glBindTexture(render_target->gl_texture_target, 0));
	GL_CHECK_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

Framebuffer* RenderDevice::create_framebuffer(const FramebufferCreateDesc& desc)
{
	Framebuffer* framebuffer = new Framebuffer();
    
    framebuffer->num_render_targets = 0;

	GL_CHECK_ERROR(glGenFramebuffers(1, &framebuffer->id));

	for (int i = 0; i < desc.num_render_targets; i++)
		attach_render_target(framebuffer, desc.render_targets[i]);

	if (desc.depth_target)
		attach_depth_stencil_target(framebuffer, desc.depth_target);

	return framebuffer;
}


VertexBuffer* RenderDevice::create_vertex_buffer(const BufferCreateDesc& desc)
{
	VertexBuffer* buffer = new VertexBuffer();
	GL_CHECK_ERROR(glGenBuffers(1, &buffer->id));

	GLenum glusageType = kBufferUsageTable[desc.usage_type];

	buffer->buffer_type = GL_ARRAY_BUFFER;
	buffer->data = desc.data;
	buffer->size = desc.size;
	buffer->usage_type = glusageType;

	return buffer;
}

UniformBuffer* RenderDevice::create_uniform_buffer(const BufferCreateDesc& desc)
{
	UniformBuffer* buffer = new UniformBuffer();
	GL_CHECK_ERROR(glGenBuffers(1, &buffer->id));

	GLenum glusageType = kBufferUsageTable[desc.usage_type];

	GL_CHECK_ERROR(glBindBuffer(GL_UNIFORM_BUFFER, buffer->id));
	GL_CHECK_ERROR(glBufferData(GL_UNIFORM_BUFFER, desc.size, desc.data, glusageType));
	GL_CHECK_ERROR(glBindBuffer(GL_UNIFORM_BUFFER, 0));

	buffer->buffer_type = GL_UNIFORM_BUFFER;
	buffer->data = desc.data;
	buffer->size = desc.size;
	buffer->usage_type = glusageType;

	return buffer;
}

IndexBuffer* RenderDevice::create_index_buffer(const BufferCreateDesc& desc)
{
	IndexBuffer* buffer = new IndexBuffer();
	GL_CHECK_ERROR(glGenBuffers(1, &buffer->id));

	GLenum glusageType = kBufferUsageTable[desc.usage_type];

	buffer->buffer_type = GL_ELEMENT_ARRAY_BUFFER;
	buffer->data = desc.data;
	buffer->size = desc.size;
	buffer->usage_type = glusageType;
	buffer->type = kBufferDataTypeTable[desc.data_type];

	return buffer;
}

InputLayout* RenderDevice::create_input_layout(const InputLayoutCreateDesc& desc)
{
    InputLayout* ia  = new InputLayout();
    
    memcpy(&ia->elements[0], &desc.elements[0], sizeof(InputElement) * desc.num_elements);
    
    ia->num_elements = desc.num_elements;
    ia->vertex_size  = desc.vertex_size;
    
    return ia;
}

VertexArray* RenderDevice::create_vertex_array(const VertexArrayCreateDesc& desc)
{
	VertexArray* vertexArray = new VertexArray();
	vertexArray->ib = desc.index_buffer;
	vertexArray->vb = desc.vertex_buffer;

	GL_CHECK_ERROR(glGenVertexArrays(1, &vertexArray->id));
	GL_CHECK_ERROR(glBindVertexArray(vertexArray->id));

	GL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, desc.vertex_buffer->id));
	GL_CHECK_ERROR(glBufferData(GL_ARRAY_BUFFER, desc.vertex_buffer->size, desc.vertex_buffer->data, desc.vertex_buffer->usage_type));

    if(desc.index_buffer)
    {
        GL_CHECK_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, desc.index_buffer->id));
        GL_CHECK_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER, desc.index_buffer->size, desc.index_buffer->data, desc.index_buffer->usage_type));
    }
	
	for (uint32_t i = 0; i < desc.layout->num_elements; i++)
	{
		GL_CHECK_ERROR(glEnableVertexAttribArray(i));
		GL_CHECK_ERROR(glVertexAttribPointer(i, 
										     desc.layout->elements[i].num_sub_elements,
											 kBufferDataTypeTable[desc.layout->elements[i].type],
											 desc.layout->elements[i].normalized,
											 desc.layout->vertex_size,
											 (GLvoid*)((uint64_t)desc.layout->elements[i].offset)));
	}

	GL_CHECK_ERROR(glBindVertexArray(0));
	GL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_CHECK_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

	return vertexArray;
}

Texture2D* RenderDevice::create_texture_2d(const Texture2DCreateDesc& desc)
{
	Texture2D* texture = new Texture2D();

	GL_CHECK_ERROR(glGenTextures(1, &texture->id));
	texture->gl_texture_target = GL_TEXTURE_2D;

	GL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, texture->id));

	GLenum internalFormat, format, type;

	internalFormat = kTextureFormatTable[desc.format][0];
    format = kTextureFormatTable[desc.format][1];
	type = kTextureFormatTable[desc.format][2];
    // @TODO: Handle formats. GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_RED_INTEGER, GL_RG_INTEGER, GL_RGB_INTEGER, GL_BGR_INTEGER,
    // GL_RGBA_INTEGER, GL_BGRA_INTEGER, GL_STENCIL_INDEX, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL

	GL_CHECK_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, desc.width, desc.height, 0, format, type, desc.data));

	if (desc.generate_mipmaps)
	{
		GL_CHECK_ERROR(glGenerateMipmap(GL_TEXTURE_2D));
	}

	GL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, 0));

	return texture;
}

RasterizerState* RenderDevice::create_rasterizer_state(const RasterizerStateCreateDesc& desc)
{
	RasterizerState* rasterizerState = new RasterizerState();

	rasterizerState->cull_face = kCullModeTable[desc.cull_mode];
	rasterizerState->enable_cull_face = desc.cull_mode != CullMode::NONE;
	rasterizerState->polygon_mode = kFillModeTable[desc.fill_mode];
	rasterizerState->enable_multisample = desc.multisample;
	rasterizerState->enable_scissor = desc.scissor;
	rasterizerState->enable_front_face_ccw = desc.front_winding_ccw;

	return rasterizerState;
}

DepthStencilState* RenderDevice::create_depth_stencil_state(const DepthStencilStateCreateDesc& desc)
{
	DepthStencilState* depthStencilState = new DepthStencilState();

	// Set Depth Options

	depthStencilState->enable_depth = desc.enable_depth_test;
	depthStencilState->depth_func = kComparisonFunctionTable[desc.depth_cmp_func];
	depthStencilState->depth_mask = (desc.depth_mask) ? GL_TRUE : GL_FALSE;

	// Set Stencil Options

	depthStencilState->enable_stencil = desc.enable_stencil_test;
	depthStencilState->front_stencil_comparison = kComparisonFunctionTable[0];
	depthStencilState->back_stencil_comparison = kComparisonFunctionTable[desc.back_stencil_cmp_func];

	// Front Stencil Operation

	depthStencilState->front_stencil_fail = kStencilOperationTable[desc.front_stencil_fail];
	depthStencilState->front_stencil_pass_depth_pass = kStencilOperationTable[desc.front_stencil_pass_depth_pass];
	depthStencilState->front_stencil_pass_depth_fail = kStencilOperationTable[desc.front_stencil_pass_depth_fail];

	// Back Stencil Operation

	depthStencilState->back_stencil_fail = kStencilOperationTable[desc.back_stencil_fail];
	depthStencilState->back_stencil_pass_depth_pass = kStencilOperationTable[desc.back_stencil_pass_depth_pass];
	depthStencilState->back_stencil_pass_depth_fail = kStencilOperationTable[desc.back_stencil_pass_depth_fail];
	depthStencilState->stencil_mask = desc.stencil_mask;

	return depthStencilState;
}

PipelineStateObject* RenderDevice::create_pipeline_state_object(const PipelineStateObjectCreateDesc& desc)
{
	PipelineStateObject* pso = new PipelineStateObject();

	pso->depth_stencil_state = create_depth_stencil_state(desc.depth_stencil_state);
	pso->rasterizer_state = create_rasterizer_state(desc.rasterizer_state);
	pso->primitive = desc.primitive;

	return pso;
}

SamplerState* RenderDevice::create_sampler_state(const SamplerStateCreateDesc& desc)
{
	SamplerState* samplerState = new SamplerState();

	GL_CHECK_ERROR(glGenSamplers(1, &samplerState->id));

	GL_CHECK_ERROR(glSamplerParameteri(samplerState->id, GL_TEXTURE_WRAP_S, kTextureWrapModeTable[desc.wrap_mode_u]));

	if (desc.wrap_mode_u == TextureWrapMode::CLAMP_TO_BORDER)
	{
		GLfloat borderColor[] = { desc.border_color[0], desc.border_color[1], desc.border_color[2], desc.border_color[3] };
		GL_CHECK_ERROR(glSamplerParameterfv(samplerState->id, GL_TEXTURE_BORDER_COLOR, borderColor));
	}

	GL_CHECK_ERROR(glSamplerParameteri(samplerState->id, GL_TEXTURE_WRAP_T, kTextureWrapModeTable[desc.wrap_mode_v]));

	if (desc.wrap_mode_v == TextureWrapMode::CLAMP_TO_BORDER)
	{
		GLfloat borderColor[] = { desc.border_color[0], desc.border_color[1], desc.border_color[2], desc.border_color[3] };
		GL_CHECK_ERROR(glSamplerParameterfv(samplerState->id, GL_TEXTURE_BORDER_COLOR, borderColor));
	}

	GL_CHECK_ERROR(glSamplerParameteri(samplerState->id, GL_TEXTURE_WRAP_R, kTextureWrapModeTable[desc.wrap_mode_w]));

	if (desc.wrap_mode_w == TextureWrapMode::CLAMP_TO_BORDER)
	{
		GLfloat borderColor[] = { desc.border_color[0], desc.border_color[1], desc.border_color[2], desc.border_color[3] };
		GL_CHECK_ERROR(glSamplerParameterfv(samplerState->id, GL_TEXTURE_BORDER_COLOR, borderColor));
	}

	// Texture Filtering

    if ((desc.min_filter == TextureFilteringMode::ANISOTROPIC_ALL || desc.mag_filter == TextureFilteringMode::ANISOTROPIC_ALL) && desc.max_anisotropy > 0)
    {
        if (GL_EXT_texture_filter_anisotropic)
        {
            GLfloat glmaxAnisotropy;
            GL_CHECK_ERROR(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &glmaxAnisotropy));

            if (desc.max_anisotropy <= glmaxAnisotropy) {
                GL_CHECK_ERROR(glSamplerParameterf(samplerState->id, GL_TEXTURE_MAX_ANISOTROPY_EXT, desc.max_anisotropy));
            }
            else {
                GL_CHECK_ERROR(glSamplerParameterf(samplerState->id, GL_TEXTURE_MAX_ANISOTROPY_EXT, glmaxAnisotropy));
            }
        }
        else
        {
            GL_CHECK_ERROR(glSamplerParameteri(samplerState->id, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            GL_CHECK_ERROR(glSamplerParameteri(samplerState->id, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        }
    }
    else
    {
        GL_CHECK_ERROR(glSamplerParameteri(samplerState->id, GL_TEXTURE_MAG_FILTER, kTextureMagFilteringModeTable[desc.mag_filter]));
        GL_CHECK_ERROR(glSamplerParameteri(samplerState->id, GL_TEXTURE_MIN_FILTER, kTextureMinFilteringModeTable[desc.min_filter]));
    }

	return samplerState;
}

void RenderDevice::destroy_shader(Shader* shader)
{
	if (shader)
	{
		GL_CHECK_ERROR(glDeleteShader(shader->id));
		delete shader;
	}
}

void RenderDevice::destroy_shader_program(ShaderProgram* program)
{
	if (program)
	{
		GL_CHECK_ERROR(glDeleteProgram(program->id));
		delete program;
	}
}

void RenderDevice::destroy_vertex_buffer(VertexBuffer* vertex_buffer)
{
	if (vertex_buffer)
	{
		GL_CHECK_ERROR(glDeleteBuffers(1, &vertex_buffer->id));
		delete vertex_buffer;
	}
}

void RenderDevice::destroy_index_buffer(IndexBuffer* index_buffer)
{
	if (index_buffer)
	{
		GL_CHECK_ERROR(glDeleteBuffers(1, &index_buffer->id));
		delete index_buffer;
	}
}

void RenderDevice::destroy_uniform_buffer(UniformBuffer* buffer)
{
	GL_CHECK_ERROR(glDeleteBuffers(1, &buffer->id));
	delete buffer;
}

void RenderDevice::destroy_vertex_array(VertexArray* vertex_array)
{
	if (vertex_array)
	{
		GL_CHECK_ERROR(glDeleteVertexArrays(1, &vertex_array->id));
		delete vertex_array;
	}
}

void RenderDevice::destroy_texture(Texture* texture)
{
	if (texture)
	{
		GL_CHECK_ERROR(glDeleteTextures(1, &texture->id));
		delete texture;
	}
}

void RenderDevice::destroy_rasterizer_state(RasterizerState* state)
{
	delete state;
}

void RenderDevice::destroy_sampler_state(SamplerState* state)
{
	GL_CHECK_ERROR(glDeleteSamplers(1, &state->id));
	delete state;
}

void RenderDevice::destroy_depth_stencil_state(DepthStencilState* state)
{
	delete state;
}

void RenderDevice::destroy_pipeline_state_object(PipelineStateObject* pso)
{
	destroy_depth_stencil_state(pso->depth_stencil_state);
	destroy_rasterizer_state(pso->rasterizer_state);

	delete pso;
}

void RenderDevice::destroy_framebuffer(Framebuffer* framebuffer)
{
	if (framebuffer)
	{
		GL_CHECK_ERROR(glDeleteFramebuffers(1, &framebuffer->id));

		for (int i = 0; i < framebuffer->num_render_targets; i++)
			destroy_texture(framebuffer->render_targets[i]);

		if (framebuffer->depth_target)
			destroy_texture(framebuffer->depth_target);

		delete framebuffer;
	}
}

void RenderDevice::bind_pipeline_state_object(PipelineStateObject* pso)
{
	bind_depth_stencil_state(pso->depth_stencil_state);
	bind_rasterizer_state(pso->rasterizer_state);
	set_primitive_type(pso->primitive);
}

int RenderDevice::UniformBufferAlignment()
{
	GLint uniformBufferAlignSize = 0;
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBufferAlignSize);
	return uniformBufferAlignSize;
}

void RenderDevice::bind_texture(Texture* texture, uint32_t shader_stage, uint32_t buffer_slot)
{
	if (m_device_data.last_sampler_location != GL_INVALID_INDEX)
	{
		GL_CHECK_ERROR(glBindTexture(texture->gl_texture_target, texture->id));
	}
}

void RenderDevice::bind_uniform_buffer(UniformBuffer* uniform_buffer, uint32_t shader_stage, uint32_t buffer_slot)
{
	GL_CHECK_ERROR(glBindBufferBase(GL_UNIFORM_BUFFER, buffer_slot, uniform_buffer->id));
}

void RenderDevice::bind_uniform_buffer_range(UniformBuffer* uniform_buffer, uint32_t shader_stage, uint32_t buffer_slot, size_t offset, size_t size)
{
	GL_CHECK_ERROR(glBindBufferRange(GL_UNIFORM_BUFFER, buffer_slot, uniform_buffer->id, offset, size));
}

void RenderDevice::bind_vertex_array(VertexArray* vertex_array)
{
	m_device_data.current_index_buffer = vertex_array->ib;
	GL_CHECK_ERROR(glBindVertexArray(vertex_array->id));
}

void RenderDevice::bind_rasterizer_state(RasterizerState* state)
{
	if (state->enable_cull_face)
    {
        glEnable(GL_CULL_FACE);
        GL_CHECK_ERROR(glCullFace(state->cull_face));
    }
	else
		glDisable(GL_CULL_FACE);

	GL_CHECK_ERROR(glPolygonMode(GL_FRONT_AND_BACK, state->polygon_mode));

	if (state->enable_multisample)
		glEnable(GL_MULTISAMPLE);
	else
		glDisable(GL_MULTISAMPLE);

	if (state->enable_scissor)
		glEnable(GL_SCISSOR_TEST);
	else
		glDisable(GL_SCISSOR_TEST);

	if (state->enable_front_face_ccw)
		glFrontFace(GL_CCW);
	else
		glFrontFace(GL_CW);
}

void RenderDevice::bind_sampler_state(SamplerState* state, uint32_t shader_stage, uint32_t slot)
{
	GLuint location = m_device_data.current_program->shader_map[shader_stage]->sampler_bindings[slot];
	m_device_data.last_sampler_location = location;

	if (location != GL_INVALID_INDEX)
	{
		GL_CHECK_ERROR(glActiveTexture(GL_TEXTURE0 + slot));
		GL_CHECK_ERROR(glBindSampler(slot, state->id));
		GL_CHECK_ERROR(glUniform1i(m_device_data.current_program->shader_map[shader_stage]->sampler_bindings[slot], slot));
	}
}

void RenderDevice::bind_framebuffer(Framebuffer* framebuffer)
{
	if (framebuffer)
	{
		GL_CHECK_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id));
	}
	else
	{
		GL_CHECK_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
}

void RenderDevice::bind_depth_stencil_state(DepthStencilState* state)
{
	// Set Depth Options

	if (state->enable_depth)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	GL_CHECK_ERROR(glDepthFunc(state->depth_func));
	GL_CHECK_ERROR(glDepthMask((state->depth_mask) ? GL_TRUE : GL_FALSE));

	// Set Stencil Options

	if (state->enable_stencil)
		glEnable(GL_STENCIL_TEST);
	else
		glDisable(GL_STENCIL_TEST);

	GL_CHECK_ERROR(glStencilFuncSeparate(GL_FRONT, state->front_stencil_comparison, 1, state->stencil_mask));
	GL_CHECK_ERROR(glStencilFuncSeparate(GL_FRONT, state->back_stencil_comparison, 1, state->stencil_mask));

	// Front Stencil Operation

	GL_CHECK_ERROR(glStencilOpSeparate(GL_FRONT, state->front_stencil_fail, state->front_stencil_pass_depth_fail, state->front_stencil_pass_depth_pass));
	GL_CHECK_ERROR(glStencilOpSeparate(GL_BACK, state->back_stencil_fail, state->back_stencil_pass_depth_fail, state->back_stencil_pass_depth_pass));
}

void RenderDevice::bind_shader_program(ShaderProgram* program)
{
	m_device_data.current_program = program;
	GL_CHECK_ERROR(glUseProgram(program->id));
}

void* RenderDevice::map_buffer(Buffer* buffer, uint32_t type)
{
	void* ptr = nullptr;
	GL_CHECK_ERROR(glBindBuffer(buffer->buffer_type, buffer->id));
	GL_CHECK_ERROR(ptr = glMapBuffer(buffer->buffer_type, kMapUsageTable[type]));
	return ptr;
}

void RenderDevice::unmap_buffer(Buffer* buffer)
{
	GL_CHECK_ERROR(glUnmapBuffer(buffer->buffer_type));
	GL_CHECK_ERROR(glBindBuffer(buffer->buffer_type, 0));
}

void RenderDevice::set_primitive_type(uint32_t primitive)
{
	m_device_data.primitive_type = kDrawPrimitiveTypeTable[primitive];
}

void RenderDevice::clear_framebuffer(uint32_t clear_target, float* clear_color)
{
	GL_CHECK_ERROR(glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]));

	uint32_t bits = ((clear_target & ClearTarget::COLOR) == ClearTarget::COLOR) ? GL_COLOR_BUFFER_BIT   : 0;
	bits |= ((clear_target & ClearTarget::DEPTH) == ClearTarget::DEPTH)         ? GL_DEPTH_BUFFER_BIT   : 0;
	bits |= ((clear_target & ClearTarget::STENCIL) == ClearTarget::STENCIL)     ? GL_STENCIL_BUFFER_BIT : 0;

    GL_CHECK_ERROR(glClear(bits));
}

void RenderDevice::set_viewport(uint32_t width, uint32_t height, uint32_t top_left_x, uint32_t top_left_y)
{
	GL_CHECK_ERROR(glViewport(top_left_x, top_left_y, width, height));
}

void RenderDevice::draw(uint32_t first_index, uint32_t count)
{
	GL_CHECK_ERROR(glDrawArrays(m_device_data.primitive_type, first_index, count));
}

void RenderDevice::draw_indexed(uint32_t index_count)
{
    GL_CHECK_ERROR(glDrawElements(m_device_data.primitive_type,
                                  index_count,
                                  ((m_device_data.current_index_buffer) ? m_device_data.current_index_buffer->type : GL_UNSIGNED_INT),
                                  0));
}

void RenderDevice::draw_indexed_base_vertex(uint32_t index_count, uint32_t base_index, uint32_t base_vertex)
{
	GL_CHECK_ERROR(glDrawElementsBaseVertex(m_device_data.primitive_type,
											index_count, 
											((m_device_data.current_index_buffer) ? m_device_data.current_index_buffer->type : GL_UNSIGNED_INT), 
											(void*)(sizeof(unsigned int) * base_index), 
											base_vertex));
}
