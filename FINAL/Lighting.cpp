#include "pch.h"
#include "Lighting.h"
#include <cmath>

// ---- CLightSource ----

CLightSource::CLightSource()
{
	posX = posY = posZ = 0;
	colorR = colorG = colorB = 1.0f;
	type = LIGHT_POINT;
	intensity = 1.0f;
}

void CLightSource::SetPointLight(float x, float y, float z, float r, float g, float b, float intens)
{
	posX = x; posY = y; posZ = z;
	colorR = r; colorG = g; colorB = b;
	type = LIGHT_POINT;
	intensity = intens;
}

void CLightSource::SetDirectional(float dx, float dy, float dz, float r, float g, float b, float intens)
{
	float len = sqrtf(dx * dx + dy * dy + dz * dz);
	posX = dx / len; posY = dy / len; posZ = dz / len;
	colorR = r; colorG = g; colorB = b;
	type = LIGHT_DIRECTIONAL;
	intensity = intens;
}

// ---- CMaterial ----

CMaterial::CMaterial()
{
	ambientR = ambientG = ambientB = 0.1f;
	diffuseR = diffuseG = diffuseB = 0.8f;
	specularR = specularG = specularB = 0.5f;
	shininess = 32.0f;
}

void CMaterial::SetDefaultRed()
{
	ambientR = 0.15f; ambientG = 0.0f; ambientB = 0.0f;
	diffuseR = 0.8f; diffuseG = 0.1f; diffuseB = 0.1f;
	specularR = 0.6f; specularG = 0.3f; specularB = 0.3f;
	shininess = 64.0f;
}

void CMaterial::SetFromMTL(float kdR, float kdG, float kdB)
{
	ambientR = kdR * 0.15f; ambientG = kdG * 0.15f; ambientB = kdB * 0.15f;
	diffuseR = kdR; diffuseG = kdG; diffuseB = kdB;
	specularR = specularG = specularB = 0.3f;
	shininess = 32.0f;
}

// ---- CLighting ----

float CLighting::Clamp01(float v)
{
	return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}

void CLighting::ComputeVertexLight(
	float worldX, float worldY, float worldZ,
	float normalX, float normalY, float normalZ,
	float viewPosX, float viewPosY, float viewPosZ,
	const CMaterial* pMat,
	const CLightSource* pLight,
	float& outR, float& outG, float& outB)
{
	if (!pMat || !pLight)
	{
		outR = outG = outB = 0.5f;
		return;
	}

	// Light direction
	float lx, ly, lz;
	if (pLight->type == LIGHT_DIRECTIONAL)
	{
		lx = -pLight->posX; ly = -pLight->posY; lz = -pLight->posZ;
	}
	else
	{
		lx = pLight->posX - worldX;
		ly = pLight->posY - worldY;
		lz = pLight->posZ - worldZ;
	}
	float lLen = sqrtf(lx * lx + ly * ly + lz * lz);
	if (lLen > 1e-10f) { lx /= lLen; ly /= lLen; lz /= lLen; }

	float NdotL = normalX * lx + normalY * ly + normalZ * lz;
	float diff = Clamp01(NdotL);

	// View direction
	float vx = viewPosX - worldX;
	float vy = viewPosY - worldY;
	float vz = viewPosZ - worldZ;
	float vLen = sqrtf(vx * vx + vy * vy + vz * vz);
	if (vLen > 1e-10f) { vx /= vLen; vy /= vLen; vz /= vLen; }

	// Reflection direction
	float rx = 2.0f * NdotL * normalX - lx;
	float ry = 2.0f * NdotL * normalY - ly;
	float rz = 2.0f * NdotL * normalZ - lz;

	float RdotV = rx * vx + ry * vy + rz * vz;
	float spec = powf(Clamp01(RdotV), pMat->shininess);

	// Attenuation for point light
	float atten = 1.0f;
	if (pLight->type == LIGHT_POINT)
		atten = 1.0f / (1.0f + 0.1f * lLen + 0.01f * lLen * lLen);

	float liR = pLight->colorR * pLight->intensity * atten;
	float liG = pLight->colorG * pLight->intensity * atten;
	float liB = pLight->colorB * pLight->intensity * atten;

	outR = pMat->ambientR * 0.2f + pMat->diffuseR * liR * diff + pMat->specularR * liR * spec;
	outG = pMat->ambientG * 0.2f + pMat->diffuseG * liG * diff + pMat->specularG * liG * spec;
	outB = pMat->ambientB * 0.2f + pMat->diffuseB * liB * diff + pMat->specularB * liB * spec;

	outR = Clamp01(outR);
	outG = Clamp01(outG);
	outB = Clamp01(outB);
}
