

#define MAX_INSTANCES 815

cbuffer uniformBlock : register(b0, UPDATE_FREQ_PER_DRAW)
{
	float4x4 mvp;

	float4 color[MAX_INSTANCES];
    // Point Light Information
	float4 lightPosition;
	float4 lightColor;
    
	float4x4 toWorld[MAX_INSTANCES];
};

struct VSInput
{
	float4 Position : POSITION;
	float4 Normal : NORMAL;
};

struct VSOutput
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
};

VSOutput main(VSInput input, uint InstanceID : SV_InstanceID)
{
	VSOutput result;
	float4x4 tempMat = mul(mvp, toWorld[InstanceID]);
	result.Position = mul(tempMat, input.Position);

	float4 normal = normalize(mul(toWorld[InstanceID], float4(input.Normal.xyz, 0.0f))); // Assume uniform scaling
	float4 pos = mul(toWorld[InstanceID], float4(input.Position.xyz, 1.0f));

	float lightIntensity = 1.0f;
	float quadraticCoeff = 1.2;
	float ambientCoeff = 0.4;

	float3 lightDir = normalize(lightPosition.xyz - pos.xyz);

	float distance = length(lightDir);
	float attenuation = 1.0 / (quadraticCoeff * distance * distance);
	float intensity = lightIntensity * attenuation;

	float3 baseColor = color[InstanceID].xyz;
	float3 blendedColor = mul(lightColor.xyz * baseColor, lightIntensity);
	float3 diffuse = mul(blendedColor, max(dot(normal.xyz, lightDir), 0.0));
	float3 ambient = mul(baseColor, ambientCoeff);
	result.Color = float4(diffuse + ambient, 1.0);

	return result;
}
