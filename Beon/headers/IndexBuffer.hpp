//Index Buffer class
#pragma once

class IndexBuffer{
private:
	unsigned int RendererID;
	unsigned int Count;
public:
	IndexBuffer(const unsigned short* data, unsigned int count);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	inline unsigned int GetCount() const {return Count;}
};

IndexBuffer::IndexBuffer(const unsigned short* data, unsigned int count) : Count(count){ //unsigned int

	glGenBuffers(1, &RendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned short), data, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer(){
	glDeleteBuffers(1, &RendererID);
}

void IndexBuffer::Bind() const{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererID);
}

void IndexBuffer::Unbind() const{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}