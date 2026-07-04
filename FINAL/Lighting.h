#pragma once

enum LightType { LIGHT_POINT = 0, LIGHT_DIRECTIONAL = 1 };

class CLightSource
{
public:
	float posX, posY, posZ;
	float colorR, colorG, colorB;
	LightType type;
	float intensity;

	CLightSource();
	void SetPointLight(float x, float y, float z, float r, float g, float b, float intens);
	void SetDirectional(float dx, float dy, float dz, float r, float g, float b, float intens);
};

class CMaterial
{
public:
	float ambientR, ambientG, ambientB;
	float diffuseR, diffuseG, diffuseB;
	float specularR, specularG, specularB;
	float shininess;

	CMaterial();
	void SetDefaultRed();
	void SetFromMTL(float kdR, float kdG, float kdB);
};

class CLighting
{
public:
	static void ComputeVertexLight(
		float worldX, float worldY, float worldZ,
		float normalX, float normalY, float normalZ,
		float viewPosX, float viewPosY, float viewPosZ,
		const CMaterial* pMat,
		const CLightSource* pLight,
		float& outR, float& outG, float& outB);

private:
	static float Clamp01(float v);
};
