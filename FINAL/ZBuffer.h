#pragma once

class CZBuffer
{
public:
	CZBuffer();
	~CZBuffer();

	void Create(int width, int height);
	void Destroy();
	void Clear();

	inline bool TestAndSet(int x, int y, float z)
	{
		if (x < 0 || x >= m_width || y < 0 || y >= m_height) return false;
		int idx = y * m_width + x;
		if (z < m_pBuffer[idx]) { m_pBuffer[idx] = z; return true; }
		return false;
	}

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

private:
	float* m_pBuffer;
	int m_width, m_height;
};
