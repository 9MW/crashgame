
#include "BasicStructures.fxh"

Texture2D g_Texture;
SamplerState g_Texture_sampler; // By convention, texture samplers must use the '_sampler' suffix

cbuffer cbCameraAttribs
{
	CameraAttribs g_CameraAttribs;
};
cbuffer cScreenRect {
	Selects para;
};

//cbuffer cbSelect
//{
//	Selects sele;
//};
//draw selection rect
void vsele(in uint VertexID : SV_VertexID, out v2f o) {
	const float4 mn = para.maxin;
	float4 Pos[4];
	Pos[0] = float4(mn.zw, 0.0, 1.0);
	Pos[1] = float4(mn.z, mn.y, 0.0, 1.0);
	Pos[2] = float4(mn.x, mn.w, 0.0, 1.0);
	Pos[3] = float4(mn.xy, 0.0, 1.0);
	o.color = para.org;
	float2 UV[4];
	UV[0] = float2(+0.0, +1.0);
	UV[1] = float2(+0.0, +0.0);
	UV[2] = float2(+1.0, +1.0);
	UV[3] = float2(+1.0, +0.0);
	o.uv = UV[VertexID];
	o.vertex = Pos[VertexID];
}
float4 fsele(in v2f i) : SV_Target{
return i.color;
}

void fowv(in VertexInput1 v,
	out v2f PSOut)
{
	//uint2 pos = (uint2) round(v.vertex);
	//uint4 sd = g_ShadowMap[pos];
	//objrep InstanceID = curProp[v.instance];

	float4 posWorld = (float4(v.vertex, 1));
	PSOut.vertex = mul(posWorld, g_CameraAttribs.mViewProj);
	PSOut.wps = posWorld;
	PSOut.uv = v.UV0;
}
float4 fowf(in v2f i) : SV_Target
{
	//uint2 rdres = (uint2) ((round(i.wps / g_CameraAttribs.f4ExtraData[0]) + g_CameraAttribs.f4ExtraData[1]).xy);
	return g_Texture.Sample(g_Texture_sampler, i.uv);
//(lightMapfow.Sample(fowresult_sampler, i.uv)) + 1;
// Dfsap2D(fowresult, i.color.xy);
}