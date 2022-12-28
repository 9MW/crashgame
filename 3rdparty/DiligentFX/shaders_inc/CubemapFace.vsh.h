"cbuffer cbTransform\n"
"{\n"
"    float4x4 g_Rotation;\n"
"}\n"
"\n"
"void main(in uint VertexId     : SV_VertexID,\n"
"          out float4 Pos       : SV_Position,\n"
"          out float3 WorldPos  : WORLD_POS)\n"
"{\n"
"    float2 PosXY[4];\n"
"    PosXY[0] = float2(-1.0, -1.0);\n"
"    PosXY[1] = float2(-1.0, +1.0);\n"
"    PosXY[2] = float2(+1.0, -1.0);\n"
"    PosXY[3] = float2(+1.0, +1.0);\n"
"    Pos = float4(PosXY[VertexId], 1.0, 1.0);\n"
"    float4 f4WorldPos = mul(g_Rotation, Pos);\n"
"    WorldPos = f4WorldPos.xyz / f4WorldPos.w;\n"
"#if (defined(GLSL) || defined(GL_ES)) && !defined(VULKAN)\n"
"    Pos.y *= -1.0;\n"
"#endif\n"
"}\n"
