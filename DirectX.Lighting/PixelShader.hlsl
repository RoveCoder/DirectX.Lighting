#include "Header.hlsli"

float4 main(PixelInput input) : SV_TARGET
{
	input.Normal = normalize(input.Normal);

	// Sample Texture
	float4 diffuse_texture = TextureDiffuse.Sample(SamplerAnisotropic, input.Texture);

	// Diffuse lighting
	float3 lightVec = mDirectionalLight.mDirection.xyz;
	float4 diffuse_light = saturate(dot(lightVec, input.Normal)) * mDirectionalLight.mDiffuse * mMaterial.mDiffuse;

	// Ambient lighting
	float4 ambient_light = mDirectionalLight.mAmbient * mMaterial.mAmbient;

	// Specular lighting
	float3 viewDir = normalize(mDirectionalLight.mCameraPos - input.Position);
	float3 reflectDir = reflect(-lightVec, input.Normal);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), mDirectionalLight.mSpecular.w * mMaterial.mSpecular.w);

	float4 specular_light = float4(spec * mDirectionalLight.mSpecular.xyz * mMaterial.mSpecular.xyz, 1.0f);

	// Combine all pixels
	float4 finalColour = (diffuse_light + ambient_light + specular_light) * diffuse_texture;
	return finalColour;
}