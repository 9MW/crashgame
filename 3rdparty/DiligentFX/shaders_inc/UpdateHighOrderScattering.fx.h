"\n"
"#ifndef THREAD_GROUP_SIZE\n"
"#   define THREAD_GROUP_SIZE 16\n"
"#endif\n"
"\n"
"\n"
"Texture3D<float3> g_tex3DCurrentOrderScattering;\n"
"Texture3D<float3> g_tex3DHighOrderOrderScattering;\n"
"\n"
"RWTexture3D</*format = rgba16f*/float3> g_rwtex3DHighOrderSctr;\n"
"\n"
"[numthreads(THREAD_GROUP_SIZE, THREAD_GROUP_SIZE, 1)]\n"
"void UpdateHighOrderScatteringCS(uint3 ThreadId  : SV_DispatchThreadID)\n"
"{\n"
"    // Accumulate in-scattering using alpha-blending\n"
"    g_rwtex3DHighOrderSctr[ThreadId] = \n"
"        g_tex3DHighOrderOrderScattering.Load( int4(ThreadId, 0) ) + \n"
"        g_tex3DCurrentOrderScattering.Load( int4(ThreadId, 0) );\n"
"}\n"
