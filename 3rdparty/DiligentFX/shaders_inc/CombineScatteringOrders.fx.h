"\n"
"#ifndef THREAD_GROUP_SIZE\n"
"#   define THREAD_GROUP_SIZE 16\n"
"#endif\n"
"\n"
"Texture3D<float3> g_tex3DSingleSctrLUT;\n"
"Texture3D<float3> g_tex3DHighOrderSctrLUT;\n"
"\n"
"RWTexture3D</*format = rgba16f*/float3> g_rwtex3DMultipleSctr;\n"
"\n"
"[numthreads(THREAD_GROUP_SIZE, THREAD_GROUP_SIZE, 1)]\n"
"void CombineScatteringOrdersCS(uint3 ThreadId  : SV_DispatchThreadID)\n"
"{\n"
"    // Combine single & higher order scattering into single look-up table\n"
"    g_rwtex3DMultipleSctr[ThreadId] = \n"
"                     g_tex3DSingleSctrLUT.Load( int4(ThreadId, 0) ).xyz + \n"
"                     g_tex3DHighOrderSctrLUT.Load( int4(ThreadId, 0) ).xyz;\n"
"}\n"