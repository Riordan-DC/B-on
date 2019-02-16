//Vertex buffer class
#pragma once


class VertexBuffer{
private:
	unsigned int RendererID;
public:
	VertexBuffer();
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
	void Build(const void* data, unsigned int size);
};

VertexBuffer::VertexBuffer(){}

VertexBuffer::VertexBuffer(const void* data, unsigned int size){ //std::vector<glm::vec3>
	glGenBuffers(1, &RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, RendererID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer(){
	glDeleteBuffers(1, &RendererID);
}

void VertexBuffer::Bind() const{
	glBindBuffer(GL_ARRAY_BUFFER, RendererID);
}

void VertexBuffer::Unbind() const{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::Build(const void* data, unsigned int size){
	glGenBuffers(1, &RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, RendererID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}