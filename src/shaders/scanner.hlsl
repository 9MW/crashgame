#include "BasicStructures.fxh"
Texture2D<float4> lightMap;
cbuffer Constants
{
    GlobalConstants g_Constants;
};
[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void fov(uint3 Gid: SV_GroupID, uint3 GTid: SV_GroupThreadID)
{
    uint uiGlobalThreadIdx = Gid.x * THREAD_GROUP_SIZE / 8 + uint(GTid.x / 8);
    if (uiGlobalThreadIdx >= g_Constants.uiNum)
        return; 

}