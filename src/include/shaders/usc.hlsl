
#include "BasicStructures.fxh"
#include "VTYPES.fxh"

Texture2D<TLIT> lightMapfow;
RWTexture2D<TUSMAP> usMap,blx,bly;
SamplerState lightMapfow_sampler;
#ifndef OUTPUT_TEXTURE_SIZE
#define OUTPUT_TEXTURE_SIZE 4096
#endif
#define TG3(x,y,z) x,y,z
#define TGDF TG3(32, 32, 1)
#define COMPAT_SamplePoint(tex, uv) lightMapfow.Sample(lightMapfow_sampler, uv).x
#define OUTPUT_TEXTURE_SIZE_Inv (1.0 / 4096.0)
cbuffer Cbuf
{
    CameraAttribs g_CameraAttribs;
};

const static float3 dt = float3(1.0, 1.0, 1.0);
const float mx = 0;       // start smoothing wt.
const float k = -0.5;      // wt. decrease factor
const float max_w = 100;   // max filter weigth
const float min_w = 0.03;   // min filter weigth
const float lum_add = 100;  // effects smoothing
const float2 texbeg = 0.25;
//scalaHQ 4x
[numthreads(TGDF)]
void uschq(uint3 Gid: SV_GroupID,
         uint3 GTid: SV_GroupThreadID) {
    float2 ps = g_CameraAttribs.f4ViewportSize.zw;
    uint2 texoutcoord = min(OUTPUT_TEXTURE_SIZE - 1, (Gid * uint3(TGDF) + GTid).xy);
    float2 texCoord = texoutcoord * OUTPUT_TEXTURE_SIZE_Inv;//*0.5+texbeg;
    float x = 0.5 * OUTPUT_TEXTURE_SIZE_Inv*4;//4 times smaller than up scaling texture
    float y = 0.5 * OUTPUT_TEXTURE_SIZE_Inv*4;
    float2 dg1 = float2(x, y);
    float2 dg2 = float2(-x, y);
    float2 sd1 = dg1 * 0.5;
    float2 sd2 = dg2 * 0.5;
    float2 ddx = float2(x, 0.0);
    float2 ddy = float2(0.0, y);

    float4 t1 = float4(texCoord - sd1, texCoord - ddy);
    float4 t2 = float4(texCoord - sd2, texCoord + ddx);
    float4 t3 = float4(texCoord + sd1, texCoord + ddy);
    float4 t4 = float4(texCoord + sd2, texCoord - ddx);
    float4 t5 = float4(texCoord - dg1, texCoord - dg2);
    float4 t6 = float4(texCoord + dg1, texCoord + dg2);

    float c = COMPAT_SamplePoint(decal, texCoord);
    float i1 = COMPAT_SamplePoint(decal, t1.xy);
    float i2 = COMPAT_SamplePoint(decal, t2.xy);
    float i3 = COMPAT_SamplePoint(decal, t3.xy);
    float i4 = COMPAT_SamplePoint(decal, t4.xy);
    float o1 = COMPAT_SamplePoint(decal, t5.xy);
    float o3 = COMPAT_SamplePoint(decal, t6.xy);
    float o2 = COMPAT_SamplePoint(decal, t5.zw);
    float o4 = COMPAT_SamplePoint(decal, t6.zw);
    float s1 = COMPAT_SamplePoint(decal, t1.zw);
    float s2 = COMPAT_SamplePoint(decal, t2.zw);
    float s3 = COMPAT_SamplePoint(decal, t3.zw);
    float s4 = COMPAT_SamplePoint(decal, t4.zw);

    float ko1 = dot(abs(o1 - c), dt);
    float ko2 = dot(abs(o2 - c), dt);
    float ko3 = dot(abs(o3 - c), dt);
    float ko4 = dot(abs(o4 - c), dt);

    float k1 = min(dot(abs(i1 - i3), dt), max(ko1, ko3));
    float k2 = min(dot(abs(i2 - i4), dt), max(ko2, ko4));

    float w1 = k2; if (ko3 < ko1) w1 *= ko3 / ko1;
    float w2 = k1; if (ko4 < ko2) w2 *= ko4 / ko2;
    float w3 = k2; if (ko1 < ko3) w3 *= ko1 / ko3;
    float w4 = k1; if (ko2 < ko4) w4 *= ko2 / ko4;

    c = (w1 * o1 + w2 * o2 + w3 * o3 + w4 * o4 + 0.001 * c) / (w1 + w2 + w3 + w4 + 0.001);
    w1 = k * dot(abs(i1 - c) + abs(i3 - c), dt) / (0.125 * dot(i1 + i3, dt) + lum_add);
    w2 = k * dot(abs(i2 - c) + abs(i4 - c), dt) / (0.125 * dot(i2 + i4, dt) + lum_add);
    w3 = k * dot(abs(s1 - c) + abs(s3 - c), dt) / (0.125 * dot(s1 + s3, dt) + lum_add);
    w4 = k * dot(abs(s2 - c) + abs(s4 - c), dt) / (0.125 * dot(s2 + s4, dt) + lum_add);

    w1 = clamp(w1 + mx, min_w, max_w);
    w2 = clamp(w2 + mx, min_w, max_w);
    w3 = clamp(w3 + mx, min_w, max_w);
    w4 = clamp(w4 + mx, min_w, max_w);

    usMap[texoutcoord] = (w1 * (i1 + i3) + w2 * (i2 + i4) + w3 * (s1 + s3) + w4 * (s2 + s4) + c) / (2.0 * (w1 + w2 + w3 + w4) + 1.0);
}

[numthreads(TGDF)]
void usc(uint3 Gid: SV_GroupID,
         uint3 GTid: SV_GroupThreadID) {
    float2 ps = g_CameraAttribs.f4ViewportSize.zw;
    uint2 texoutcoord = min(OUTPUT_TEXTURE_SIZE - 1, (Gid * uint3(TGDF) + GTid).xy);
    float2 texCoord = texoutcoord * OUTPUT_TEXTURE_SIZE_Inv; //*0.5+texbeg;
    float x = 0.5 * OUTPUT_TEXTURE_SIZE_Inv * 4;             //4 times smaller than up scaling texture
    float y = 0.5 * OUTPUT_TEXTURE_SIZE_Inv * 4;
    float2 ddx = float2(x, 0.0);
    float2 ddy = float2(0.0, y);
    float2 dg1 = ddx * 2;
    float2 dg2 = -ddy;
    float2 sd1 = dg1 * 1.5;
    float2 sd2 = dg2 * 1.5;

    float4 t1 = float4(texCoord - sd1, texCoord - ddy);
    float4 t2 = float4(texCoord - sd2, texCoord + ddx);
    float4 t3 = float4(texCoord + sd1, texCoord + ddy);
    float4 t4 = float4(texCoord + sd2, texCoord - ddx);
    float4 t5 = float4(texCoord - dg1, texCoord - dg2);
    float4 t6 = float4(texCoord + dg1, texCoord + dg2);

    float c = COMPAT_SamplePoint(decal, texCoord);
    float i1 = COMPAT_SamplePoint(decal, t1.xy);
    float i2 = COMPAT_SamplePoint(decal, t2.xy);
    float i3 = COMPAT_SamplePoint(decal, t3.xy);
    float i4 = COMPAT_SamplePoint(decal, t4.xy);
    float o1 = COMPAT_SamplePoint(decal, t5.xy);
    float o3 = COMPAT_SamplePoint(decal, t6.xy);
    float o2 = COMPAT_SamplePoint(decal, t5.zw);
    float o4 = COMPAT_SamplePoint(decal, t6.zw);
    float s1 = COMPAT_SamplePoint(decal, t1.zw);
    float s2 = COMPAT_SamplePoint(decal, t2.zw);
    float s3 = COMPAT_SamplePoint(decal, t3.zw);
    float s4 = COMPAT_SamplePoint(decal, t4.zw);
    usMap[texoutcoord] =(c+
i1+
i2+
i3+
i4+
o1+
o3+
o2+
o4+
s1+
s2+
s3+
s4
    )/13;
}
[numthreads(64,1,1)]
void gfx(uint3 Gid: SV_GroupID,
         uint3 GTid: SV_GroupThreadID) {
    uint2 texoutcoord = min(OUTPUT_TEXTURE_SIZE - 1, (Gid * uint3(TGDF) + GTid).xy);
    float2 texCoord = texoutcoord * OUTPUT_TEXTURE_SIZE_Inv * 0.5 + texbeg;
    blx[texoutcoord] =usMap[texCoord];
}
[numthreads(64, 1, 1)]
void gfy(uint3 Gid: SV_GroupID,
         uint3 GTid: SV_GroupThreadID) {
    uint2 texoutcoord = min(OUTPUT_TEXTURE_SIZE - 1, (Gid * uint3(TGDF) + GTid).xy);
    float2 texCoord = texoutcoord * OUTPUT_TEXTURE_SIZE_Inv * 0.5 + texbeg;
    blx[texoutcoord] = usMap[texCoord];
}