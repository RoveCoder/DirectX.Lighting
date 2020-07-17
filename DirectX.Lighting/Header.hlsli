cbuffer WorldBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	matrix InverseWorld;
}

struct VertexInput
{
	float3 Position : POSITION;
	float2 Texture : TEXCOORD0;
	float3 Normal : NORMAL;
};

struct PixelInput
{
	float3 Position : POSITION;
	float4 PositionH : SV_POSITION;
	float2 Texture : TEXCOORD0;
	float3 Normal : NORMAL;
};

SamplerState SamplerAnisotropic : register(s0);

Texture2D TextureDiffuse : register(t0);