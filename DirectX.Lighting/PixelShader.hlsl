#include "Header.hlsli"

float4 main(PixelInput input) : SV_TARGET
{
	input.Normal = normalize(input.Normal);

	// Sample Texture
	float4 diffuse_texture = TextureDiffuse.Sample(SamplerAnisotropic, input.Texture);

	// Diffuse lighting
	float3 lightVec = mDirectionalLight.mDirection.xyz;
	float4 diffuse_light = saturate(dot(lightVec, input.Normal)) * mDirectionalLight.mDiffuse;// *gDiffuse;

	// Ambient lighting
	float4 ambient_light = mDirectionalLight.mAmbient;// *gAmbient;


	// Combine all pixels
	float4 finalColour = (diffuse_light + ambient_light) * diffuse_texture;
	return finalColour;
}