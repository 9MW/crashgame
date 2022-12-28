
inline float4 QuaternionMul(float4 v,const float4 q)
{
    float3 t = 2 * cross(v.xyz, -q.xyz);
    v.xyz = v.xyz + q.w * t + cross(t, -q.xyz);
    return v;
}
inline float4 QuaternionInv(in float4 v)
{
    return v*float4(-1,-1,-1,1);
}
