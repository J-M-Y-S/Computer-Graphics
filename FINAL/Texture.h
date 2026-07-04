#pragma once

class CTexture
{
public:
	CTexture();
	~CTexture();

	bool Load(const CString& filePath);

	DWORD SampleBilinear(float u, float v) const;
	DWORD SampleNearest(float u, float v) const;

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	bool IsLoaded() const { return m_pData != nullptr; }

private:
	BYTE* m_pData;
	int m_width, m_height;
};
