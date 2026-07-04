#include "pch.h"
#include "ZBuffer.h"

CZBuffer::CZBuffer() : m_pBuffer(nullptr), m_width(0), m_height(0) {}

CZBuffer::~CZBuffer() { Destroy(); }

void CZBuffer::Create(int width, int height)
{
	if (m_width == width && m_height == height && m_pBuffer) return;
	Destroy();
	m_width = width;
	m_height = height;
	m_pBuffer = new float[width * height];
}

void CZBuffer::Destroy()
{
	delete[] m_pBuffer;
	m_pBuffer = nullptr;
	m_width = m_height = 0;
}

void CZBuffer::Clear()
{
	if (!m_pBuffer) return;
	int size = m_width * m_height;
	for (int i = 0; i < size; i++)
		m_pBuffer[i] = 1.0f;
}
