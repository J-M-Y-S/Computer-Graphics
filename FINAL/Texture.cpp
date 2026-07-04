#include "pch.h"
#include "Texture.h"
#include <wincodec.h>

#pragma comment(lib, "windowscodecs.lib")

CTexture::CTexture() : m_pData(nullptr), m_width(0), m_height(0) {}

CTexture::~CTexture()
{
	delete[] m_pData;
	m_pData = nullptr;
}

bool CTexture::Load(const CString& filePath)
{
	delete[] m_pData;
	m_pData = nullptr;
	m_width = m_height = 0;

	IWICImagingFactory* pFactory = nullptr;
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr,
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFactory));
	if (FAILED(hr)) return false;

	IWICBitmapDecoder* pDecoder = nullptr;
	hr = pFactory->CreateDecoderFromFilename(filePath, nullptr,
		GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder);
	if (FAILED(hr)) { pFactory->Release(); return false; }

	IWICBitmapFrameDecode* pFrame = nullptr;
	hr = pDecoder->GetFrame(0, &pFrame);
	if (FAILED(hr)) { pDecoder->Release(); pFactory->Release(); return false; }

	IWICFormatConverter* pConverter = nullptr;
	hr = pFactory->CreateFormatConverter(&pConverter);
	if (FAILED(hr))
	{
		pFrame->Release(); pDecoder->Release(); pFactory->Release();
		return false;
	}

	hr = pConverter->Initialize(pFrame, GUID_WICPixelFormat32bppRGBA,
		WICBitmapDitherTypeNone, nullptr, 0.0f,
		WICBitmapPaletteTypeMedianCut);
	if (FAILED(hr))
	{
		pConverter->Release(); pFrame->Release();
		pDecoder->Release(); pFactory->Release();
		return false;
	}

	UINT w = 0, h = 0;
	pConverter->GetSize(&w, &h);
	m_width = (int)w;
	m_height = (int)h;

	UINT stride = w * 4;
	UINT bufSize = stride * h;
	m_pData = new BYTE[bufSize];

	hr = pConverter->CopyPixels(nullptr, stride, bufSize, m_pData);

	pConverter->Release();
	pFrame->Release();
	pDecoder->Release();
	pFactory->Release();

	return SUCCEEDED(hr);
}

DWORD CTexture::SampleNearest(float u, float v) const
{
	if (!m_pData) return 0xFF808080;

	u = u - floorf(u);
	v = v - floorf(v);

	int x = (int)(u * m_width) % m_width;
	int y = (int)(v * m_height) % m_height;
	if (x < 0) x += m_width;
	if (y < 0) y += m_height;

	int idx = (y * m_width + x) * 4;
	BYTE b = m_pData[idx];
	BYTE g = m_pData[idx + 1];
	BYTE r = m_pData[idx + 2];
	BYTE a = m_pData[idx + 3];

	return (a << 24) | (r << 16) | (g << 8) | b;
}

DWORD CTexture::SampleBilinear(float u, float v) const
{
	if (!m_pData) return 0xFF808080;

	u = u - floorf(u);
	v = v - floorf(v);

	float fu = u * m_width - 0.5f;
	float fv = v * m_height - 0.5f;

	int x0 = (int)floorf(fu);
	int y0 = (int)floorf(fv);
	int x1 = x0 + 1;
	int y1 = y0 + 1;

	float fracU = fu - x0;
	float fracV = fv - y0;

	x0 = ((x0 % m_width) + m_width) % m_width;
	x1 = ((x1 % m_width) + m_width) % m_width;
	y0 = ((y0 % m_height) + m_height) % m_height;
	y1 = ((y1 % m_height) + m_height) % m_height;

	auto Sample = [this](int x, int y) -> DWORD {
		int idx = (y * m_width + x) * 4;
		return (m_pData[idx + 3] << 24) | (m_pData[idx + 2] << 16)
			| (m_pData[idx + 1] << 8) | m_pData[idx];
		};

	DWORD c00 = Sample(x0, y0);
	DWORD c10 = Sample(x1, y0);
	DWORD c01 = Sample(x0, y1);
	DWORD c11 = Sample(x1, y1);

	auto LerpByte = [](BYTE a, BYTE b, float t) -> BYTE {
		return (BYTE)(a + (b - a) * t);
		};

	BYTE r00 = (c00 >> 16) & 0xFF, g00 = (c00 >> 8) & 0xFF, b00 = c00 & 0xFF, a00 = (c00 >> 24) & 0xFF;
	BYTE r10 = (c10 >> 16) & 0xFF, g10 = (c10 >> 8) & 0xFF, b10 = c10 & 0xFF, a10 = (c10 >> 24) & 0xFF;
	BYTE r01 = (c01 >> 16) & 0xFF, g01 = (c01 >> 8) & 0xFF, b01 = c01 & 0xFF, a01 = (c01 >> 24) & 0xFF;
	BYTE r11 = (c11 >> 16) & 0xFF, g11 = (c11 >> 8) & 0xFF, b11 = c11 & 0xFF, a11 = (c11 >> 24) & 0xFF;

	BYTE r0 = LerpByte(r00, r10, fracU), r1 = LerpByte(r01, r11, fracU);
	BYTE g0 = LerpByte(g00, g10, fracU), g1 = LerpByte(g01, g11, fracU);
	BYTE b0 = LerpByte(b00, b10, fracU), b1 = LerpByte(b01, b11, fracU);
	BYTE a0 = LerpByte(a00, a10, fracU), a1 = LerpByte(a01, a11, fracU);

	BYTE r = LerpByte(r0, r1, fracV);
	BYTE g = LerpByte(g0, g1, fracV);
	BYTE b = LerpByte(b0, b1, fracV);
	BYTE a = LerpByte(a0, a1, fracV);

	return (a << 24) | (r << 16) | (g << 8) | b;
}
