#include "renderer.h"

#include <array>
#include <fstream>

#include <glad/glad.h>
#include <stb_image.h>
#include <glm/gtc/type_ptr.hpp>

Ref<VertexBuffer> VertexBuffer::CreateVertexBuffer(uint64_t size, const void* data) {
	Ref<VertexBuffer> buffer = Ref<VertexBuffer>(new VertexBuffer(size));
	if (data) buffer->updateBuffer(size, data);
	return buffer;
}

Ref<IndexBuffer> IndexBuffer::CreateIndexBuffer(uint64_t size, const uint32_t* data) {
	Ref<IndexBuffer> buffer = Ref<IndexBuffer>(new IndexBuffer(size));
	if (data) buffer->updateBuffer(size, data);
	return buffer;
}

Ref<VertexArray> VertexArray::CreateVertexArray(const VertexLayout& layout, const std::shared_ptr<VertexBuffer> buffer) {
	Ref<VertexArray> vertArray = Ref<VertexArray>(new VertexArray());
	if (buffer) vertArray->addVertexBuffer(layout, buffer);
	return vertArray;
}

Ref<ShaderProgram> ShaderProgram::Create(const std::string& vertPath, const std::string& fragPath) {
	std::string vertSrc = readFile(vertPath);
	std::string fragSrc = readFile(fragPath);
	Ref<ShaderProgram> program = Ref<ShaderProgram>(new ShaderProgram(vertSrc, fragSrc));
	return program;
}

Ref<Texture> Texture::CreateTexture(const std::string& filepath) {
	Ref<Texture> texture = Ref<Texture>(new Texture(filepath));
	return texture;
}

/************************************************************************************************************************/
/*          BEGIN Texture                                                                                               */
/************************************************************************************************************************/

Texture::~Texture() {
	glDeleteTextures(1, &this->id);
}

void Texture::bind(uint32_t slot) {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, this->id);
}

Texture::Texture(const std::string& filepath) {
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	stbi_uc* data = nullptr;
	data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
	assert(data);

	int internalFormat = 0;
	int dataFormat = 0;
	if (channels == 4) {
		internalFormat = GL_RGBA8;
		dataFormat = GL_RGBA;
	}
	else if (channels == 3) {
		internalFormat = GL_RGB8;
		dataFormat = GL_RGB;
	}

	assert(internalFormat & dataFormat);

	glGenTextures(1, &this->id);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->id);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	stbi_image_free(data);
}

/************************************************************************************************************************/
/*          BEGIN VertexBuffer                                                                                          */
/************************************************************************************************************************/

VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1, &this->id);
}

void VertexBuffer::bind() { glBindBuffer(GL_ARRAY_BUFFER, this->id); }

void VertexBuffer::updateBuffer(uint64_t size, const void* subdata, uint64_t offset) {
	assert(size + offset >= size && size + offset >= offset && size + offset <= this->size);
	glBindBuffer(GL_ARRAY_BUFFER, this->id);
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, subdata);
}

VertexBuffer::VertexBuffer(uint64_t size) : size(size) {
	glGenBuffers(1, &this->id);
	glBindBuffer(GL_ARRAY_BUFFER, this->id);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/************************************************************************************************************************/
/*          BEGIN IndexBuffer                                                                                           */
/************************************************************************************************************************/

IndexBuffer::~IndexBuffer() {
	glDeleteBuffers(1, &this->id);
}

void IndexBuffer::bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id); }

void IndexBuffer::updateBuffer(uint64_t size, const uint32_t* subdata, uint64_t offset) {
	assert(size + offset >= size && size + offset >= offset && size + offset <= this->size);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, subdata);
}

IndexBuffer::IndexBuffer(uint64_t size) : size(size), count(size / sizeof(uint32_t)) {
	glGenBuffers(1, &this->id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/************************************************************************************************************************/
/*          BEGIN VertexArray                                                                                           */
/************************************************************************************************************************/

uint64_t ShaderDataType::GetSize(Type type) {
	switch (type) {
	case Type::FLOAT: case Type::INT: case Type::BOOL:
		return 4;
	case Type::FLOAT2: case Type::INT2: case Type::BOOL2:
		return 8;
	case Type::FLOAT3: case Type::INT3: case Type::BOOL3:
		return 12;
	case Type::FLOAT4: case Type::INT4: case Type::BOOL4:
		return 16;
	case Type::MAT3:
		return 36;
	case Type::MAT4:
		return 64;
	default:
		throw "Unknown ShaderDataType::Type in GetSize";
	}
}

uint64_t ShaderDataType::GetCount(Type type) {
	switch (type) {
	case Type::FLOAT: case Type::INT: case Type::BOOL:
		return 1;
	case Type::FLOAT2: case Type::INT2: case Type::BOOL2:
		return 2;
	case Type::FLOAT3: case Type::INT3: case Type::BOOL3:
		return 3;
	case Type::FLOAT4: case Type::INT4: case Type::BOOL4:
		return 4;
	case Type::MAT3:
		return 9;
	case Type::MAT4:
		return 16;
	default:
		throw "Unknown ShaderDataType::Type in GetCount";
	}
}

LayoutElement::LayoutElement(const char* name, ShaderDataType::Type type, bool normalized)
	: type(type), normalized(normalized), offset(0) {}

VertexLayout::VertexLayout() : stride(0) {}
VertexLayout::VertexLayout(const std::initializer_list<LayoutElement>& elements)
	: elements(elements) {
	calculateOffsetAndStride();
}

void VertexLayout::calculateOffsetAndStride() {
	stride = 0;
	for (auto& element : elements) {
		element.offset = stride;
		stride += ShaderDataType::GetSize(element.type);
	}
}

VertexArray::~VertexArray() {
	glDeleteVertexArrays(1, &this->id);
}

void VertexArray::bind() {
	glBindVertexArray(this->id);
}

void VertexArray::addVertexBuffer(const VertexLayout& layout, const std::shared_ptr<VertexBuffer> vertices) {
	glBindVertexArray(this->id);
	vertices->bind();
	uint32_t index = 0;
	for (const auto& element : layout) {
		switch (element.type) {
		case ShaderDataType::Type::FLOAT:
		case ShaderDataType::Type::FLOAT2:
		case ShaderDataType::Type::FLOAT3:
		case ShaderDataType::Type::FLOAT4:
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(
				index, ShaderDataType::GetCount(element.type), GL_FLOAT,
				element.normalized ? GL_TRUE : GL_FALSE,
				layout.getStride(), (const void*)element.offset
			);
			index++;
			break;

		case ShaderDataType::Type::MAT3:
			for (uint64_t j = 0; j < 36; j += 12) {
				glEnableVertexAttribArray(index);
				glVertexAttribPointer(
					index, 3, GL_FLOAT, element.normalized ? GL_TRUE : GL_FALSE,
					layout.getStride(), (const void*)(element.offset + j)
				);
				index++;
			}
			break;
		case ShaderDataType::Type::MAT4:
			for (uint64_t j = 0; j < 64; j += 16) {
				glEnableVertexAttribArray(index);
				glVertexAttribPointer(
					index, 4, GL_FLOAT, element.normalized ? GL_TRUE : GL_FALSE,
					layout.getStride(), (const void*)(element.offset + j)
				);
				index++;
			}
			break;
		default:
			glEnableVertexAttribArray(index);
			glVertexAttribIPointer(
				index, ShaderDataType::GetCount(element.type),
				GL_INT, layout.getStride(), (const void*)element.offset
			);
			index++;
			break;
		}
	}
}

VertexArray::VertexArray() {
	glGenVertexArrays(1, &this->id);
}

/************************************************************************************************************************/
/*          BEGIN ShaderProgram                                                                                         */
/************************************************************************************************************************/

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(this->id);
}

void ShaderProgram::bind() { glUseProgram(this->id); }

void ShaderProgram::uploadFloat(const std::string& name, const float f) const {
	int location = glGetUniformLocation(this->id, name.c_str());
	glUniform1f(location, f);
}

void ShaderProgram::uploadFloat2(const std::string& name, const glm::vec2& vec) const {
	int location = glGetUniformLocation(this->id, name.c_str());
	glUniform2f(location, vec[0], vec[1]);
}

void ShaderProgram::uploadFloat3(const std::string& name, const glm::vec3& vec) const {
	int location = glGetUniformLocation(this->id, name.c_str());
	glUniform3f(location, vec[0], vec[1], vec[2]);
}

void ShaderProgram::uploadFloat4(const std::string& name, const glm::vec4& vec) const {
	int location = glGetUniformLocation(this->id, name.c_str());
	glUniform4f(location, vec[0], vec[1], vec[2], vec[3]);
}

void ShaderProgram::uploadInt(const std::string& name, const int i) const {
	int location = glGetUniformLocation(this->id, name.c_str());
	glUniform1i(location, i);
}

void ShaderProgram::uploadMat3(const std::string& name, const glm::mat3& matrix) const {
	int location = glGetUniformLocation(this->id, name.c_str());
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void ShaderProgram::uploadMat4(const std::string& name, const glm::mat4& matrix) const {
	int location = glGetUniformLocation(this->id, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

ShaderProgram::ShaderProgram(const std::string& vertSrc, const std::string& fragSrc) {
	this->id = glCreateProgram();
	uint32_t vertId = glCreateShader(GL_VERTEX_SHADER);
	compileShader(vertId, vertSrc.c_str(), vertSrc.size());
	glAttachShader(this->id, vertId);
	
	uint32_t fragId = glCreateShader(GL_FRAGMENT_SHADER);
	compileShader(fragId, fragSrc.c_str(), fragSrc.size());
	glAttachShader(this->id, fragId);

	glLinkProgram(this->id);

	int isLinked = 0;
	glGetProgramiv(this->id, GL_LINK_STATUS, &isLinked);
	if (!isLinked) {
		int maxLength = 0;
		glGetProgramiv(this->id, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<char> infoLog(maxLength);
		glGetProgramInfoLog(this->id, maxLength, &maxLength, &infoLog[0]);
		glDeleteProgram(this->id);
		glDeleteShader(vertId);
		glDeleteShader(fragId);
		throw std::string(infoLog.data());
	}
	glDeleteShader(vertId);
	glDeleteShader(fragId);
}

void ShaderProgram::compileShader(uint32_t id, const char* const src, int length) {
	glShaderSource(id, 1, &src, 0);
	glCompileShader(id);

	int status = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);

	if (!status) {
		int maxLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<char> infoLog(maxLength);
		glGetShaderInfoLog(id, maxLength, &maxLength, &infoLog[0]);
		glDeleteShader(id);
		fprintf(stderr, "%s\n", infoLog.data());
		throw std::string(infoLog.data());
	}
}

std::string ShaderProgram::readFile(const std::string& filepath) {
	std::string result;
	std::ifstream in(filepath, std::ios::in | std::ios::binary);
	if (in) {
		in.seekg(0, std::ios::end);
		result.resize((uint64_t)in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&result[0], result.size());
		in.close();
	}
	else {
		throw "Could not open file" + filepath;
	}
	return result;
}

/************************************************************************************************************************/
/*          BEGIN Renderer                                                                                              */
/************************************************************************************************************************/

#pragma pack(1)
struct Vertex {
	glm::vec2 position;
	glm::vec2 uv;
	uint32_t textureSlot;
	float transparency;
};
#pragma pack(0)

constexpr uint64_t QUAD_COUNT = 256;
constexpr uint64_t VERT_COUNT = QUAD_COUNT * 4;
constexpr uint64_t INDX_COUNT = QUAD_COUNT * 6;

glm::mat4 OrthographicProjection = glm::ortho(-360.0f / 64.0f, 360.0f / 64.0f, -240.0f / 64.0f, 240.0f / 64.0f);

uint64_t QuadCount = 0;
std::array<uint32_t, INDX_COUNT> Indices;
std::array<Vertex, VERT_COUNT> Vertices;
Ref<VertexBuffer> RenderVertexBuffer;
Ref<IndexBuffer> RenderIndexBuffer;
Ref<VertexArray> RenderVertexArray;

Ref<ShaderProgram> RenderShaderProgram;

std::array<Ref<Texture>, 4> RenderTextures{nullptr};

void RendererInit() {
	RenderVertexBuffer = VertexBuffer::CreateVertexBuffer(VERT_COUNT * sizeof(Vertex));
	
	for (uint64_t i = 0; i < QUAD_COUNT; i++) {
		for (uint64_t j = 0; j < 3; j++) {
			Indices[i * 6 + j] = j + (i * 4);
			Indices[i * 6 + j + 3] = (2 + j + (i * 4));
		}
		Indices[i * 6 + 5] = i * 4;
	}

	RenderIndexBuffer = IndexBuffer::CreateIndexBuffer(INDX_COUNT * sizeof(uint32_t), Indices.data());
	RenderVertexArray = VertexArray::CreateVertexArray(
		{
			{ "position", ShaderDataType::Type::FLOAT2 },
			{ "uv", ShaderDataType::Type::FLOAT2 },
			{ "textureSlot", ShaderDataType::Type::INT },
			{ "transparency", ShaderDataType::Type::FLOAT }
		},
		RenderVertexBuffer
	);

	RenderShaderProgram = ShaderProgram::Create("./resources/shaders/board.vert", "./resources/shaders/board.frag");
	RenderShaderProgram->bind();
	RenderShaderProgram->uploadInt("light_piece", 0);
	RenderShaderProgram->uploadInt("dark_piece", 1);
	RenderShaderProgram->uploadInt("light_board", 2);
	RenderShaderProgram->uploadInt("dark_board", 3);

	RenderTextures[(uint64_t) Textures::LIGHT_PIECE] = Texture::CreateTexture("./resources/textures/light_piece.png");
	RenderTextures[(uint64_t) Textures::DARK_PIECE] = Texture::CreateTexture("./resources/textures/dark_piece.png");
	RenderTextures[(uint64_t) Textures::LIGHT_BOARD] = Texture::CreateTexture("./resources/textures/light_board.png");
	RenderTextures[(uint64_t) Textures::DARK_BOARD] = Texture::CreateTexture("./resources/textures/dark_board.png");

	for (int i = 0; i < 4; i++) {
		RenderTextures[i]->bind(i);
	}
}

void RenderQuad(const glm::vec2& position, Textures texture, float transparency) {
	if (QuadCount >= QUAD_COUNT) {
		RendererFlush();
	}
	const Vertex vertices[4] = {
		{ position + glm::vec2{  0.f,  0.f }, {0.0f, 0.0f}, (uint32_t)texture, transparency},
		{ position + glm::vec2{  0.f, -1.f }, {0.0f, 1.0f}, (uint32_t) texture, transparency },
		{ position + glm::vec2{  1.f, -1.f }, {1.0f, 1.0f}, (uint32_t) texture, transparency },
		{ position + glm::vec2{  1.f,  0.f }, {1.0f, 0.0f}, (uint32_t) texture, transparency }
	};

	Vertices[QuadCount++] = vertices[0];
	Vertices[QuadCount++] = vertices[1];
	Vertices[QuadCount++] = vertices[2];
	Vertices[QuadCount++] = vertices[3];
}

void RendererFlush() {
	if (QuadCount == 0) return;
	RenderVertexBuffer->updateBuffer(QuadCount * 4 * sizeof(Vertex), Vertices.data());
	RenderVertexArray->bind();
	RenderIndexBuffer->bind();
	RenderShaderProgram->bind();

	RenderShaderProgram->uploadMat4("u_Projection", OrthographicProjection);

	glDrawElements(GL_TRIANGLES, (QuadCount / 4) * 6, GL_UNSIGNED_INT, NULL);
	QuadCount = 0;
}

void RendererShutdown() {
	RenderVertexBuffer = nullptr;
	RenderIndexBuffer = nullptr;
	RenderVertexArray = nullptr;
	RenderShaderProgram = nullptr;
	for (int i = 0; i < 4; i++) RenderTextures[i] = nullptr;
	QuadCount = 0;
}
