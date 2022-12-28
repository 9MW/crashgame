
//#pragma SHADER_TARGET 5.1
#define DfTex(name) Texture2D name;SamplerState name##_sampler
#define DfSap(name,uv) name.Sample(name##_sampler,uv)
DfTex(_boneTexture);

//DfTex(g_tex2DDiffuse);
Texture2D g_tex2DDiffuse;
SamplerState g_tex2DDiffuse_sampler;

int _boneTextureBlockWidth;
int _boneTextureBlockHeight;
int _boneTextureWidth;
int _boneTextureHeight;
int _frameOff;
//#ifndef ANIMATION_INSTANCING_BASE
//#define ANIMATION_INSTANCING_BASE
//#define UNITY_DEFINE_INSTANCED_PROP(type, var)  type var;
//#define UNITY_ACCESS_INSTANCED_PROP(arr, var)   arr##Array[InstanceID].var
//#define SHADER_TARGET 50
//#define tex2Dlod(tex,uv) tex

//#if (SHADER_TARGET < 30 || SHADER_API_GLES)
//uniform float frameIndex;
//uniform float preFrameIndex;
//uniform float transitionProgress;
//#else
//UNITY_INSTANCING_CBUFFER_START(Props)
//	UNITY_DEFINE_INSTANCED_PROP(float, preFrameIndex)
//	UNITY_DEFINE_INSTANCED_PROP(float, frameIndex)
//	UNITY_DEFINE_INSTANCED_PROP(float, transitionProgress)
//UNITY_INSTANCING_CBUFFER_END
//#endif
struct appdata_full
{
    float4 vertex : POSITION;
    float4 tangent : TANGENT;
    float3 normal : NORMAL;
    float4 texcoord : TEXCOORD0;
    float4 texcoord1 : TEXCOORD1;
    float4 texcoord2 : TEXCOORD2;
    float4 texcoord3 : TEXCOORD3;
    float4 color : COLOR;
    uint InstID : SV_InstanceID;
};
//float4x2 loadTrsFromTexture(uint frameIndex, uint boneIndex)
//{
//    uint blockCount = _boneTextureWidth / _boneTextureBlockWidth;
//    int2 uv;
//    uv.y = frameIndex / blockCount * _boneTextureBlockHeight;
//    uv.x = _boneTextureBlockWidth * (frameIndex - _boneTextureWidth / _boneTextureBlockWidth * uv.y);

//    int transCount = _boneTextureBlockWidth /2;
//    uv.x = uv.x + (boneIndex % transCount) * 4;
//    uv.y = uv.y + boneIndex / transCount;

//    float2 uvFrame;
//    uvFrame.x = uv.x / (float) _boneTextureWidth;
//    uvFrame.y = uv.y / (float) _boneTextureHeight;
//    float4 uvf = float4(uvFrame, 0, 0);

//    float offset = 1.0f / (float) _boneTextureWidth;
   
//    float4 c1 = _boneTexture.Sample(_boneTexture_sampler, uvf);
//    uvf.x = uvf.x + offset;
//    float4 c2 = _boneTexture.Sample(_boneTexture_sampler, uvf);
//    uvf.x = uvf.x + offset;
//    float4x2 m;
//    m[0] = c1;
//    m[1] = c2;
//    return m;
//}

//float4 skinning(inout appdata_full v)
//{
//    float4 w = v.color;
//    float4 bone = float4(v.texcoord2.x, v.texcoord2.y, v.texcoord2.z, v.texcoord2.w);
//    float curFrame = instDatas[v.InstID].frameIndex;
//    float preAniFrame = instDatas[v.InstID].preFrameIndex;
//    float progress = instDatas[v.InstID].transitionProgress;

//	//float curFrame = UNITY_ACCESS_INSTANCED_PROP(frameIndex);
//    int preFrame = curFrame;
//    int nextFrame = curFrame + 1.0f;
//    float4x4 localToWorldMatrixPre = loadTrsFromTexture(preFrame, bone.x) * w.x;
//    localToWorldMatrixPre += loadTrsFromTexture(preFrame, bone.y) * max(0, w.y);
//    localToWorldMatrixPre += loadTrsFromTexture(preFrame, bone.z) * max(0, w.z);
//    localToWorldMatrixPre += loadTrsFromTexture(preFrame, bone.w) * max(0, w.w);

//    float4x4 localToWorldMatrixNext = loadTrsFromTexture(nextFrame, bone.x) * w.x;
//    localToWorldMatrixNext += loadTrsFromTexture(nextFrame, bone.y) * max(0, w.y);
//    localToWorldMatrixNext += loadTrsFromTexture(nextFrame, bone.z) * max(0, w.z);
//    localToWorldMatrixNext += loadTrsFromTexture(nextFrame, bone.w) * max(0, w.w);

//    float4 localPosPre = mul(v.vertex, localToWorldMatrixPre);
//    float4 localPosNext = mul(v.vertex, localToWorldMatrixNext);
//    float4 localPos = lerp(localPosPre, localPosNext, curFrame - preFrame);

//    half3 localNormPre = mul(v.normal.xyz, (float3x3) localToWorldMatrixPre);
//    half3 localNormNext = mul(v.normal.xyz, (float3x3) localToWorldMatrixNext);
//    v.normal = normalize(lerp(localNormPre, localNormNext, curFrame - preFrame));
//    half3 localTanPre = mul(v.tangent.xyz, (float3x3) localToWorldMatrixPre);
//    half3 localTanNext = mul(v.tangent.xyz, (float3x3) localToWorldMatrixNext);
//    v.tangent.xyz = normalize(lerp(localTanPre, localTanNext, curFrame - preFrame));

//    float4x4 localToWorldMatrixPreAni = loadTrsFromTexture(preAniFrame, bone.x);
//    float4 localPosPreAni = mul(v.vertex, localToWorldMatrixPreAni);
//    localPos = lerp(localPos, localPosPreAni, (1.0f - progress) * (preAniFrame > 0.0f));
//    return localPos;
//}

//float4 skinningShadow(inout appdata_full v)
//{
//    float4 bone = float4(v.texcoord2.x, v.texcoord2.y, v.texcoord2.z, v.texcoord2.w);
//    float curFrame = instDatas[v.InstID].frameIndex;
//    float preAniFrame = instDatas[v.InstID].preFrameIndex;
//    float progress = instDatas[v.InstID].transitionProgress;

//    int preFrame = curFrame;
//    int nextFrame = curFrame + 1.0f;
//    float4x4 localToWorldMatrixPre = loadTrsFromTexture(preFrame, bone.x);
//    float4x4 localToWorldMatrixNext = loadTrsFromTexture(nextFrame, bone.x);
//    float4 localPosPre = mul(v.vertex, localToWorldMatrixPre);
//    float4 localPosNext = mul(v.vertex, localToWorldMatrixNext);
//    float4 localPos = lerp(localPosPre, localPosNext, curFrame - preFrame);
//    float4x4 localToWorldMatrixPreAni = loadTrsFromTexture(preAniFrame, bone.x);
//    float4 localPosPreAni = mul(v.vertex, localToWorldMatrixPreAni);
//    localPos = lerp(localPos, localPosPreAni, (1.0f - progress) * (preAniFrame > 0.0f));
//	//float4 localPos = v.vertex;
//    return localPos;
//}

//void vert(inout appdata_full v)
//{
//#ifdef UNITY_PASS_SHADOWCASTER
//	v.vertex = skinningShadow(v);
//#else
//    v.vertex = skinning(v);
//#endif
//}
//#include"AnimationInstancingBase.hlsl"
#include "BasicStructures.fxh"

#define DfTex2D(name) Texture2D name;SamplerState name##_sampler
#define DfSap2D(name,uv) name.Sample(name##_sampler,uv)
//struct v2f
//{
//    float2 uv : TEXCOORD0;
//    float4 color : TEXCOORD1;
//    float4 vertex : SV_POSITION;
//};
struct InstanceProPerty
{
    float4 color;
};

sampler2D _AnimationTexture0;
sampler2D _AnimationTexture1;
sampler2D _AnimationTexture2;


StructuredBuffer<instData> instDatas;
StructuredBuffer<float4x4> animreco;
StructuredBuffer<mtransfrom> objectTransfromBuffer;
inline float4 QuaternionMul(float4 v, float4 q)
{
    float3 t = 2 * cross(v.xyz, q.xyz);
    v.xyz = v.xyz + q.w * t + cross(t, q.xyz);
    return v;
}

struct VertexInputSkinningm
{
    
    float3 vertex : ATTRIB0;
    float3 Normal : ATTRIB1;
    float2 UV0 : ATTRIB2;
    float2 UV1 : ATTRIB3;
    float4 mBoneIndices : ATTRIB4;
    float4 mBoneWeights : ATTRIB5;
    uint instance : SV_INSTANCEID;
    uint VertId : SV_VertexID;
};

struct cdata
{
    uint base, frameSize;
};
cbuffer cbCameraAttribs
{
    CameraAttribs g_CameraAttribs;
};

cbuffer cbAnimAttribs
{
    cdata AnimAttribs;
};

static matrix Identity =
{
    { 1, 0, 0, 0 },
    { 0, 1, 0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, 0, 1 }
};
DfTex2D(g_ColorMap);
inline float4x4 loadAni(in uint base, in float boneid)
{
    //if (boneid > 18 || boneid < 0)
    //{
    //    return Identity;
    //}
    return animreco[base+ int(boneid)];
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
void vert(in VertexInputSkinningm v, out v2f o)
{ 

    //o.vertex = v.VertId / 3 + Pos[v.VertId % 3];
   
    o.uv = v.UV0;
    //Pos[v.VertId % 3];
    uint InstanceID = v.instance;
    //v.mBoneWeights = float4(0.25, 0.25, 0.25, 0.25);
    uint bs = instDatas[InstanceID].base +instDatas[InstanceID].frameIndex * AnimAttribs.frameSize + AnimAttribs.base;
    float4x4 ani =
      v.mBoneWeights.x*loadAni(bs,v.mBoneIndices.x)
     +v.mBoneWeights.y*loadAni(bs,v.mBoneIndices.y)
     +v.mBoneWeights.z*loadAni(bs,v.mBoneIndices.z)
     +v.mBoneWeights.w*loadAni(bs,v.mBoneIndices.w);
    float4 posWorld = mul(ani, float4(v.vertex, 1));
    posWorld.xyz = posWorld.xyz / posWorld.w;
    //o.color =ani * posWorld*bs;
				//skinnedVertex *= objectPositionsBuffer[InstanceID].w;
    //v.vertex.xy *= float2(2, 4);
    posWorld = QuaternionMul(posWorld, objectTransfromBuffer[InstanceID].rotation);
    posWorld.xyz += objectTransfromBuffer[InstanceID].pos;
    posWorld.w = 1.0;
    //o.color.z = frac(dot(round(posWorld.xy), float2(12.9090, 78.233)));
    //o.color.z += asfloat((int) posWorld.z << 10u);
    //o.color.xy = round(posWorld.xy);
    //o.color.w = 1;
    //posWorld = (v.VertId % 100, v.VertId / 100, 0, 1);
    o.vertex = mul(posWorld, g_CameraAttribs.mViewProj);
    //o.uv = v.uv;
    //o.vertex = mul(posWorld, g_CameraAttribs.mViewProj);
	/* float3 normalSkinningRotated = mul(combinedMatrix, float4(v.normal.xyz, 0));
	float3 normalWorld = QuaternionMul(float4(normalSkinningRotated, 1), objectRotationsBuffer[InstanceID]); */
   
}

float4 frag(in v2f i) : SV_Target
{
    
    return DfSap2D(g_ColorMap, i.uv);// * 0.1 + i.color;
    //i.color;
}