
#include "BasicStructures.fxh"
#define CAM
#include "VStruct.hlsl"

#ifdef QuadCircle

StructuredBuffer<seleFilter> sele_filter;
struct VSeleInput
{
	uint VertexID : SV_VertexID;
	uint instance : SV_INSTANCEID;
};


void vert(VSeleInput VSIn, out v2f o) {
	float4 qud[4] = { float4(-1, +1, 0,1),
				 float4(-1, -1, 0,1),
				 float4(+1, +1, 0,1),
				 float4(+1, -1, 0,1) };

	float2 UV[4];
	UV[0] = float2(+0.0, +1.0);
	UV[1] = float2(+0.0, +1.0);
	UV[2] = float2(+1.0, +0.0);
	UV[3] = float2(+1.0, +1.0);
	float4 qv = qud[VSIn.VertexID];
	o.uv = UV[VSIn.VertexID];
	seleFilter sf = sele_filter[VSIn.instance];
	sf.transform.pos.z -= sf.halfextent.z;
	qv.xyz += sf.transform.pos;
	qv.xyz *= sf.transform.Scale;
	o.vertex = mul(g_CameraAttribs.mViewProj, qv);
}
float4 fsele(in v2f i) : SV_Target{
float dsq = dot(i.uv,float2(0.5,0.5));
return float4(0,1,0,max(0,1 - dsq));
}
#endif