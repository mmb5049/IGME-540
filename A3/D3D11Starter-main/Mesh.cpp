#include "Mesh.h"

Mesh::Mesh(std::string name, Vertex* vertices, int vertexCount, unsigned int* indices, int indexCount)
{
	this->name = name;
	this->vertexCount = vertexCount;
	this->indexCount = indexCount;

	// Create Vertex Buffer
	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * this->vertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialVertexData = {};
	initialVertexData.pSysMem = vertices; 
	Graphics::Device->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());

	// Create Index Buffer
	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_IMMUTABLE;	
	ibd.ByteWidth = sizeof(unsigned int) * this->indexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialIndexData = {};
	initialIndexData.pSysMem = indices;
	Graphics::Device->CreateBuffer(&ibd, &initialIndexData, indexBuffer.GetAddressOf());
}

Mesh::~Mesh()
{
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetVertexBuffer()
{
	return vertexBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetIndexBuffer()
{
	return indexBuffer;
}

int Mesh::GetIndexCount()
{
	return indexCount;
}

int Mesh::GetVertexCount()
{
	return vertexCount;
}

std::string Mesh::GetName()
{
	return name;
}

void Mesh::Draw()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	Graphics::Context->IASetVertexBuffers(
		0,
		1,
		vertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	Graphics::Context->IASetIndexBuffer(
		indexBuffer.Get(),
		DXGI_FORMAT_R32_UINT,
		0
	);

	Graphics::Context->DrawIndexed(
		indexCount,
		0,
		0
	);
}
