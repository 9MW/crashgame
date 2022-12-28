
//#include "common.hlsl"
#include "BasicStructures.fxh"
cbuffer cbCameraAttribs
{
    CameraAttribs g_CameraAttribs;
};

struct VertexInputNSkinning
{
    
    float3 vertex : ATTRIB0;
    float3 Normal : ATTRIB1;
    float4 color : ATTRIB2;
    uint instance : SV_INSTANCEID;
    uint VertId : SV_VertexID;
};
struct VSInputl
{
float4 Position: ATTRIB0;
float4 color: ATTRIB1;
};

struct PSInputl
{
    float4 Position : SV_POSITION;
    float4 color : TEX_COORD;
};

void vert(in VSInputl v,in uint VertId : SV_VertexID, out v2f o)
{
    float4 Pos[8];
    Pos[0] = float4(-1.0, -1.0, 0.0, 1.0);
    Pos[1] = float4(-1.0, +1.0, 0.0, 1.0);
    Pos[2] = float4(+1.0, -1.0, 0.0, 1.0);
    Pos[3] = float4(+1.0, +1.0, 0.0, 1.0);

    //o.vertex = v.VertId / 3 + Pos[v.VertId % 3];
   o.color=v.color;
    //Pos[v.VertId % 3];
    //v.mBoneWeights = float4(0.25, 0.25, 0.25, 0.25);
    o.wps[3]=1;//o.vertex[2];
    
    //o.color.z = frac(dot(round(objectTransfromBuffer[InstanceID].pos.xy), float2(12.9090, 78.233)));
    //o.color.z = asfloat((int) posWorld.z << 10u);
    //o.color.xy *= frac(dot(round(objectTransfromBuffer[InstanceID].pos.xy), float2(12.9090, 78.233)));;
    //o.color.a = 1;
    //posWorld = (v.VertId % 100, v.VertId / 100, 0, 1);
    //o.vertex = Pos[(VertId * 2) % 4] * 0.5;
    o.vertex = (mul(g_CameraAttribs.mViewProj,float4(v.Position.xyz,1)));
    //o.vertex.zw=float2(0,1);
    //o.uv = v.uv;
    //o.vertex = mul(posWorld, g_CameraAttribs.mViewProj);
	/* float3 normalSkinningRotated = mul(combinedMatrix, float4(v.normal.xyz, 0));
	float3 normalWorld = QuaternionMul(float4(normalSkinningRotated, 1), objectRotationsBuffer[InstanceID]); */
   
}

float4 frag(in PSInputl i) : SV_Target
{
    return i.color;
}


float2 toScreenSpace(float4 vertex)
{
    
    return float2(vertex.xy / vertex.w) * g_CameraAttribs.f4ViewportSize.xy;
}

[maxvertexcount(7)]
void main(line v2f vertices[2] , inout TriangleStream<PSInputl> triStream)
{
    float2 WIN_SCALE = g_CameraAttribs.f4ViewportSize.xy;

    //WIN_SCALE = float2(1208, 1024);

    int    xs = -1;
    float4 p0 = (vertices[1+xs].vertex);
    float4 p1 = (vertices[2+xs].vertex);
//p0.z=p1.z=0;
    float w0 = p0.w;
    float w1 = p1.w;

    p0.xyz /= p0.w;
    p1.xyz /= p1.w;

    float3 line01 = p1 - p0;
    float3 dir    = normalize(line01);

    // scale to correct window aspect ratio
    float3 ratio = float3(WIN_SCALE, 0);
    ratio        = normalize(ratio);

    float3 unit_z = normalize(float3(0, 0, -1));

    float3 normal = normalize(cross(unit_z, dir) * ratio);

    float width = 0.001;//vertices[0].color[2];

    PSInputl v[4];
    
    for (int i = 0; i < 4; i++) {
        v[i].color = vertices[i].color;
    }
    float3 dir_offset    = dir * ratio * width;
    float3 normal_scaled = normal * ratio * width;

    float3 p0_ex = p0 - dir_offset;
    float3 p1_ex = p1 + dir_offset;

    v[0].Position = float4(p0_ex - normal_scaled, 1) * w0;

    v[1].Position = float4(p0_ex + normal_scaled, 1) * w0;

    v[2].Position = float4(p1_ex + normal_scaled, 1) * w1;

    v[3].Position = float4(p1_ex - normal_scaled, 1) * w1;

    triStream.Append(v[2]);
    triStream.Append(v[1]);
    triStream.Append(v[0]);
    triStream.RestartStrip();
    triStream.Append(v[3]);
    triStream.Append(v[2]);
    triStream.Append(v[0]);
    triStream.RestartStrip();
}
