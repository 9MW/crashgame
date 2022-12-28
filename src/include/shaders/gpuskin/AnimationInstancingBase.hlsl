#define DfTex(name) Texture2D name;SamplerState name##_sampler
DfTex(_boneTexture);

cbuffer Constants
{
    int _boneTextureBlockWidth;
    int _boneTextureBlockHeight;
    int _boneTextureWidth;
    int _boneTextureHeight;
}
//#ifndef ANIMATION_INSTANCING_BASE
//#define ANIMATION_INSTANCING_BASE
//#define UNITY_DEFINE_INSTANCED_PROP(type, var)  type var;
//#define UNITY_ACCESS_INSTANCED_PROP(arr, var)   arr##Array[unity_InstanceID].var
//#define SHADER_TARGET 50
//#pragma target 3.0
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
struct instData
{
	float preFrameIndex;
	float frameIndex;
    float transitionProgress;
    float fPadding0;
};
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

StructuredBuffer<instData> instDatas;
half4x4 loadMatFromTexture(uint frameIndex, uint boneIndex)
{
	uint blockCount = _boneTextureWidth / _boneTextureBlockWidth;
	int2 uv;
	uv.y = frameIndex / blockCount * _boneTextureBlockHeight;
	uv.x = _boneTextureBlockWidth * (frameIndex - _boneTextureWidth / _boneTextureBlockWidth * uv.y);

	int matCount = _boneTextureBlockWidth / 4;
	uv.x = uv.x + (boneIndex % matCount) * 4;
	uv.y = uv.y + boneIndex / matCount;

	float2 uvFrame;
	uvFrame.x = uv.x / (float)_boneTextureWidth;
	uvFrame.y = uv.y / (float)_boneTextureHeight;
	half4 uvf = half4(uvFrame, 0, 0);

	float offset = 1.0f / (float)_boneTextureWidth;
   
    half4 c1 = _boneTexture.Sample(_boneTexture_sampler, uvf);
	uvf.x = uvf.x + offset;
    half4 c2 = _boneTexture.Sample(_boneTexture_sampler, uvf);
	uvf.x = uvf.x + offset;
    half4 c3 = _boneTexture.Sample(_boneTexture_sampler, uvf);
	uvf.x = uvf.x + offset;
	//half4 c4 = tex2Dlod(_boneTexture, uvf);
	half4 c4 = half4(0, 0, 0, 1);
    float4x4 f4;
    f4._11_12_13_14 = c4;
	//float4x4 m = float4x4(c1, c2, c3, c4);
	half4x4 m;
	m._11_21_31_41 = c1;
	m._12_22_32_42 = c2;
	m._13_23_33_43 = c3;
	m._14_24_34_44 = c4;
	return m;
}

half4 skinning(inout appdata_full v)
{
	float4 w = v.color;
	half4 bone = half4(v.texcoord2.x, v.texcoord2.y, v.texcoord2.z, v.texcoord2.w);
	float curFrame = instDatas[v.InstID].frameIndex;
    float preAniFrame = instDatas[v.InstID].preFrameIndex;
    float progress = instDatas[v.InstID].transitionProgress;

	//float curFrame = UNITY_ACCESS_INSTANCED_PROP(frameIndex);
	int preFrame = curFrame;
	int nextFrame = curFrame + 1.0f;
	half4x4 localToWorldMatrixPre = loadMatFromTexture(preFrame, bone.x) * w.x;
	localToWorldMatrixPre += loadMatFromTexture(preFrame, bone.y) * max(0, w.y);
	localToWorldMatrixPre += loadMatFromTexture(preFrame, bone.z) * max(0, w.z);
	localToWorldMatrixPre += loadMatFromTexture(preFrame, bone.w) * max(0, w.w);

	half4x4 localToWorldMatrixNext = loadMatFromTexture(nextFrame, bone.x) * w.x;
	localToWorldMatrixNext += loadMatFromTexture(nextFrame, bone.y) * max(0, w.y);
	localToWorldMatrixNext += loadMatFromTexture(nextFrame, bone.z) * max(0, w.z);
	localToWorldMatrixNext += loadMatFromTexture(nextFrame, bone.w) * max(0, w.w);

	half4 localPosPre = mul(v.vertex, localToWorldMatrixPre);
	half4 localPosNext = mul(v.vertex, localToWorldMatrixNext);
	half4 localPos = lerp(localPosPre, localPosNext, curFrame - preFrame);

	half3 localNormPre = mul(v.normal.xyz, (float3x3)localToWorldMatrixPre);
	half3 localNormNext = mul(v.normal.xyz, (float3x3)localToWorldMatrixNext);
	v.normal = normalize(lerp(localNormPre, localNormNext, curFrame - preFrame));
	half3 localTanPre = mul(v.tangent.xyz, (float3x3)localToWorldMatrixPre);
	half3 localTanNext = mul(v.tangent.xyz, (float3x3)localToWorldMatrixNext);
	v.tangent.xyz = normalize(lerp(localTanPre, localTanNext, curFrame - preFrame));

	half4x4 localToWorldMatrixPreAni = loadMatFromTexture(preAniFrame, bone.x);
	half4 localPosPreAni = mul(v.vertex, localToWorldMatrixPreAni);
	localPos = lerp(localPos, localPosPreAni, (1.0f - progress) * (preAniFrame > 0.0f));
	return localPos;
}

half4 skinningShadow(inout appdata_full v)
{
	half4 bone = half4(v.texcoord2.x, v.texcoord2.y, v.texcoord2.z, v.texcoord2.w);
    float curFrame = instDatas[v.InstID].frameIndex;
    float preAniFrame = instDatas[v.InstID].preFrameIndex;
    float progress = instDatas[v.InstID].transitionProgress;

	int preFrame = curFrame;
	int nextFrame = curFrame + 1.0f;
	half4x4 localToWorldMatrixPre = loadMatFromTexture(preFrame, bone.x);
	half4x4 localToWorldMatrixNext = loadMatFromTexture(nextFrame, bone.x);
	half4 localPosPre = mul(v.vertex, localToWorldMatrixPre);
	half4 localPosNext = mul(v.vertex, localToWorldMatrixNext);
	half4 localPos = lerp(localPosPre, localPosNext, curFrame - preFrame);
	half4x4 localToWorldMatrixPreAni = loadMatFromTexture(preAniFrame, bone.x);
	half4 localPosPreAni = mul(v.vertex, localToWorldMatrixPreAni);
	localPos = lerp(localPos, localPosPreAni, (1.0f - progress) * (preAniFrame > 0.0f));
	//half4 localPos = v.vertex;
	return localPos;
}

void vert(inout appdata_full v)
{
#ifdef UNITY_PASS_SHADOWCASTER
	v.vertex = skinningShadow(v);
#else
	v.vertex = skinning(v);
#endif
}

//#define DECLARE_VERTEX_SKINNING \
//	#pragma vertex vert 

//#endif