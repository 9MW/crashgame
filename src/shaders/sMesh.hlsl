#include "BasicStructures.fxh"

//struct v2f
//{
//    float2 uv : TEXCOORD0;
//    float4 color : TEXCOORD1;
//    float4 wps : TEXCOORD2;
//    float4 vertex : SV_POSITION;
//};
struct InstanceProPerty
{
	float4 color;
};

StructuredBuffer<instData> instDatas;
StructuredBuffer<float4x4> animreco;
StructuredBuffer<float4> map;
Texture2D<int> g_ColorMap;

StructuredBuffer<mtransfrom> objectTransfromBuffer;
//struct VertexInput1
//{

//    float3 vertex : ATTRIB0;
//    float3 Normal : ATTRIB1;
//    float2 UV0 : ATTRIB2;
//    float2 UV1 : ATTRIB3;
//    float4 mBoneIndices : ATTRIB4;
//    float4 mBoneWeights : ATTRIB5;
//    uint instance : SV_INSTANCEID;
//    uint VertId : SV_VertexID;
//};

struct cdata
{
	uint base, frameSize;
};
cbuffer cbCameraAttribs
{
	CameraAttribs g_CameraAttribs;
};


static matrix Identity =
{
	{ 1, 0, 0, 0 },
	{ 0, 1, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 0, 0, 0, 1 }
};
static float3 Quad[] = {
float3(-1,-1,0),
float3(-1,+1,0),
float3(+1,+1,0),
float3(+1,-1,0)
};
cbuffer cell1
{
	float4 cell = (4, 4, 0, 1);
};

inline float4x4 loadAni(in uint base, in float boneid)
{
	//if (boneid > 18 || boneid < 0)
	//{
	//    return Identity;
	//}
	return animreco[base + int(boneid)];
}
uint Hash(uint s)
{
	s ^= 2747636419u;
	s *= 2654435769u;
	s ^= s >> 16;
	s *= 2654435769u;
	s ^= s >> 16;
	s *= 2654435769u;
	return s;
}

float Random(uint seed)
{
	return float(Hash(seed)) / 4294967295.0; // 2^32-1
}
SamplerState _MainTex_sampler; // By convention, texture samplers must use the '_sampler' suffix
void vert(in VertexInput1 v, out v2f o)
{

	//o.vertex = v.VertId / 3 + Pos[v.VertId % 3];

	o.uv = v.UV0;
	//Pos[v.VertId % 3];
	const uint InstanceID = v.instance;
	//v.mBoneWeights = float4(0.25, 0.25, 0.25, 0.25);
	float4 posWorld = (float4(v.vertex, 1));
	//o.color =ani * posWorld*bs;
				//skinnedVertex *= objectPositionsBuffer[InstanceID].w;
	//v.vertex.xy *= float2(2, 4);
	posWorld = QuaternionMul(posWorld, objectTransfromBuffer[InstanceID].rotation);
	posWorld.xyz += objectTransfromBuffer[InstanceID].pos.xyz;
	posWorld.xyz = posWorld.xyz * objectTransfromBuffer[InstanceID].Scale.xyz;
	o.wps.xyz = (posWorld.xyz);//* 1e-20) + v.vertex.xyz;
	//o.wps.w = posWorld.w;
	//o.color.z = frac(dot(round(objectTransfromBuffer[InstanceID].pos.xy), float2(12.9090, 78.233)));
	//o.color.z = asfloat((int) posWorld.z << 10u);
	//o.color.xy *= frac(dot(round(objectTransfromBuffer[InstanceID].pos.xy), float2(12.9090, 78.233)));;
	//o.color.a = 1;
	//posWorld = (v.VertId % 100, v.VertId / 100, 0, 1);
	o.vertex = mul(g_CameraAttribs.mViewProj, posWorld);
	//o.uv = v.uv;
	//o.vertex = mul(posWorld, g_CameraAttribs.mViewProj);
	/* float3 normalSkinningRotated = mul(combinedMatrix, float4(v.normal.xyz, 0));
	float3 normalWorld = QuaternionMul(float4(normalSkinningRotated, 1), objectRotationsBuffer[InstanceID]); */

}

float4 frag(in v2f i) : SV_Target
{
    int2 rdres = round(i.wps / cell) + cell.z / 2,
         bads = round(i.wps / cell) + 512;
	int ii = rdres.x * cell.z + rdres.y;
	float4 color;
	color.rg = rdres % 2 * 0.1;
	color.g = map[ii].b;
	color.b = (g_ColorMap[bads]) / 32.0; //map[ii].b;
	color.a = 1;
	return  color;
}
//StructuredBuffer<seleout> sele_out;
StructuredBuffer<seleFilter> sele_filter;
struct VSeleInput
{
	uint VertexID : SV_VertexID;
    uint instance : SV_INSTANCEID;
};
float2 UV[] = {
 {+0.0, +1.0},
 {+0.0, +1.0},
 {+1.0, +0.0},
 {+1.0, +1.0}
};
float4 qud[] = { {-1, +1, 0, 1},
                 {-1, -1, 0, 1},
                 {+1, +1, 0, 1},
                 {+1, -1, 0, 1} };
void vsele(VSeleInput VSIn, out v2f o) {
    const uint vid = VSIn.VertexID, insid = VSIn.instance;
    float4 qv = qud[vid];
	o.uv = UV[vid];
	seleFilter sf = sele_filter[insid];
	sf.transform.pos.z -= sf.halfextent.z;
	qv.xyz += sf.transform.pos;
	qv.xyz *= sf.transform.Scale;
	o.vertex = mul(g_CameraAttribs.mViewProj, qv);
}
float4 fsele(in v2f i) : SV_Target{
float ds = distance(i.uv,float2(0.5,0.5));
return float4(0,1,0,max(0,1 - ds * ds));
}
