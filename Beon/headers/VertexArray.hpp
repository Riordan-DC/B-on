#pragma once
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"


class VertexArray{
	private:
		unsigned int RendererID;
	public:
		VertexArray();
		~VertexArray();

		void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
		void Bind() const;
		void Unbind() const;
};

VertexArray::VertexArray(){
	glGenVertexArrays(1, &RendererID);
}

VertexArray::~VertexArray(){
	glDeleteVertexArrays(1, &RendererID);
}


void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout){
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for(unsigned int i = 0; i < elements.size(); i++){
		const auto& element = elements[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*) offset);
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);;
	}
}

void VertexArray::Bind() const{
	glBindVertexArray(RendererID);
}

void VertexArray::Unbind() const{
	glBindVertexArray(0);
}

