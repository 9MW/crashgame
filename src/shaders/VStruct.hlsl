
static float4 QuadStrip[] =
{
float4(-1.0, -1.0, 0.0, 1.0),
float4(-1.0, +1.0, 0.0, 1.0),
float4(+1.0, -1.0, 0.0, 1.0),
float4(+1.0, +1.0, 0.0, 1.0),
};

static float2 QuadStripUV[] =
{
float2(+0.0, +1.0),
float2(+0.0, +0.0),
float2(+1.0, +1.0),
float2(+1.0, +0.0),
};
#ifdef CAM

cbuffer cbCameraAttribs
{
    CameraAttribs g_CameraAttribs;
};
#endif


#define DfTex(name) Texture2D name;SamplerState name##_sampler
#define DfTexs(name,num) Texture2D name[num];SamplerState name##_sampler
#define DfSap(name,uv) name.Sample(name##_sampler,uv)
#define DfSaps(name,textureId,uv) name[textureId].Sample(name##_sampler, uv)