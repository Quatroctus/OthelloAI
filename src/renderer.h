#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_set>

#include <glm/glm.hpp>

template<typename T>
using Ref = std::shared_ptr<T>;

class Texture {
public:
	static Ref<Texture> CreateTexture(const std::string& filename);
	
	~Texture();

	void bind(uint32_t slot);

private:
	uint32_t id;
	Texture(const std::string& filename);
};

struct Character {
	glm::vec4 stpq{ 0.0f };
	glm::ivec2 size{ 0 };
	glm::ivec2 offset{ 0 };
	uint32_t advance = 0;
	Character() = default;
	Character(const glm::vec4& stpq, const glm::ivec2& size, const glm::ivec2& offset, uint32_t advance);
};

class Font {
public:
	static std::shared_ptr<Font> CreateFont(const std::string& filepath, unsigned char startChar, uint32_t length);

	~Font();

	void bind(uint32_t textureSlot);

	inline const Character& getCharacterData(unsigned char c) {
		assert(c >= startChar && c < endChar);
		return characters[c];
	}

	uint32_t getTextWidth(const std::string& text);
	int32_t getTextHeight(const std::string& text);

private:
	uint32_t id;
	uint32_t startChar, endChar;
	Character* characters;
	Font(unsigned char startChar, uint32_t length);
};

class VertexBuffer {
public:
	static Ref<VertexBuffer> CreateVertexBuffer(uint64_t size, const void* data = nullptr);

	~VertexBuffer();

	void bind();
	void updateBuffer(uint64_t size, const void* data, uint64_t offset = 0);

	inline bool operator ==(const VertexBuffer& buf) { return this->id == buf.id; }

private:
	uint32_t id;
	uint64_t size;
	VertexBuffer(uint64_t size);
};

class IndexBuffer {
public:
	static Ref<IndexBuffer> CreateIndexBuffer(uint64_t size, const uint32_t* data = nullptr);

	~IndexBuffer();

	void bind();
	void updateBuffer(uint64_t size, const uint32_t* data, uint64_t offset = 0);

	inline uint64_t getCount() const { return count; }

private:
	uint32_t id;
	uint64_t size, count;
	IndexBuffer(uint64_t size);
};

namespace ShaderDataType {
	enum class Type {
		FLOAT, FLOAT2, FLOAT3, FLOAT4,
		INT, INT2, INT3, INT4,
		BOOL, BOOL2, BOOL3, BOOL4,
		MAT3, MAT4
	};

	uint64_t GetSize(Type type);
	uint64_t GetCount(Type type);
}

struct LayoutElement {
	bool normalized;
	const std::string name;
	ShaderDataType::Type type;
	uint64_t offset;
	LayoutElement(const char* name, ShaderDataType::Type type, bool normalized = false);
};

class VertexLayout {
public:
	VertexLayout();
	VertexLayout(const std::initializer_list<LayoutElement>& elements);

	inline const uint32_t getStride() const { return stride; }

	inline const std::vector<LayoutElement>& getElements() const { return elements; }

	inline std::vector<LayoutElement>::iterator begin() { return elements.begin(); }
	inline std::vector<LayoutElement>::iterator end() { return elements.end(); }

	inline std::vector<LayoutElement>::const_iterator begin() const { return elements.cbegin(); }
	inline std::vector<LayoutElement>::const_iterator end() const { return elements.cend(); }

private:
	uint64_t stride;
	std::vector<LayoutElement> elements;
	void calculateOffsetAndStride();
};


class VertexArray {
public:
	static Ref<VertexArray> CreateVertexArray(const VertexLayout& layout, const Ref<VertexBuffer> buffer);

	~VertexArray();

	void bind();
	void addVertexBuffer(const VertexLayout& layout, const Ref<VertexBuffer> vertices);

private:
	uint32_t id;
	std::unordered_set<Ref<VertexBuffer>> buffers;
	VertexArray();
};

class ShaderProgram {
public:
	static Ref<ShaderProgram> Create(const std::string& vertPath, const std::string& fragPath);

	~ShaderProgram();

	void bind();

	void uploadFloat(const std::string& name, const float f) const;
	void uploadFloat2(const std::string& name, const glm::vec2& vec) const;
	void uploadFloat3(const std::string& name, const glm::vec3& vec) const;
	void uploadFloat4(const std::string& name, const glm::vec4& vec) const;

	void uploadInt(const std::string& name, const int i) const;

	void uploadMat3(const std::string& name, const glm::mat3& matrix) const;
	void uploadMat4(const std::string& name, const glm::mat4& matrix) const;

private:
	uint32_t id;
	ShaderProgram(const std::string& vertSrc, const std::string& fragSrc);

	static std::string readFile(const std::string& filepath);
	void compileShader(uint32_t id, const char* const src, int length);

};

enum class Textures { LIGHT_PIECE, DARK_PIECE, LIGHT_BOARD, DARK_BOARD };

void RendererInit();

void RenderQuad(const glm::vec2& position, Textures texture, float transparency = 1.0f);
void RenderText(const std::string& text, const glm::vec2& position);
void RendererFlush();

void RendererShutdown();
