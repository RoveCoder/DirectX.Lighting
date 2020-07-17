#include "Header.hlsli"

float4 main(PixelInput input) : SV_TARGET
{
	return TextureDiffuse.Sample(SamplerAnisotropic, input.Texture);
}