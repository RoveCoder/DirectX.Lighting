#pragma once

#include <DirectXMath.h>

struct LightSource
{
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Specular;
};

struct DirectionalLight : public LightSource
{
	DirectX::XMFLOAT4 LightDirection;
};

struct PointLight : public LightSource
{

};