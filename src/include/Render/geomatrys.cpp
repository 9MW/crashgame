#pragma once
#include "geomatrys.h"
namespace pj2::geomatrys {
	using  Diligent::BufferData;
	using  Diligent::IRenderDevice;
	using  Diligent::IDeviceContext;
	using  Diligent::IShaderResourceBinding;
	using  Diligent::IBuffer;
	using  Diligent::IPipelineState;
	using  Diligent::RefCntAutoPtr;
	using  Diligent::USAGE;
	using  Diligent::BIND_FLAGS;
	using  Diligent::BufferDesc;
	using float3 = math::Vector3f;
	using float2 = math::Vector2f;
	using DfVertexAttr = std::tuple_element_t<0, geomatrys::vLayout>;
	template<>
	RefCntAutoPtr<IBuffer> Cubetemp<VLay::VAttribs0>::CreateVertexBuffer(IRenderDevice* pDevice, float3 ext)
	{
		// using namespace  Diligent;
		// Layout of this structure matches the one we defined in the pipeline state

		// Cube vertices

		//      (-1,+1,+1)________________(+1,+1,+1)
		//               /|              /|
		//              / |             / |
		//             /  |            /  |
		//            /   |           /   |
		//(-1,-1,+1) /____|__________/(+1,-1,+1)
		//           |    |__________|____|
		//           |   /(-1,+1,-1) |    /(+1,+1,-1)
		//           |  /            |   /
		//           | /             |  /
		//           |/              | /
		//           /_______________|/
		//        (-1,-1,-1)       (+1,-1,-1)
		//

		// clang-format off
		using VertexAttr = std::tuple_element_t<Vai, geomatrys::vLayout>;
		VertexAttr CubeVerts[] =
		{
			{ float3(-1,-1,-1),float3(0, 0, -1), float2(0,1) },
			{ float3(-1,+1,-1),float3(0, 0, -1), float2(0,0) },
			{ float3(+1,+1,-1),float3(0, 0, -1), float2(1,0) },
			{ float3(+1,-1,-1),float3(0, 0, -1), float2(1,1) },

			{ float3(-1,-1,-1),float3(0, -1, 0), float2(0,1) },
			{ float3(-1,-1,+1),float3(0, -1, 0), float2(0,0) },
			{ float3(+1,-1,+1),float3(0, -1, 0), float2(1,0) },
			{ float3(+1,-1,-1),float3(0, -1, 0), float2(1,1) },

			{ float3(+1,-1,-1),float3(+1, 0, 0), float2(0,1) },
			{ float3(+1,-1,+1),float3(+1, 0, 0), float2(1,1) },
			{ float3(+1,+1,+1),float3(+1, 0, 0), float2(1,0) },
			{ float3(+1,+1,-1),float3(+1, 0, 0), float2(0,0) },

			{ float3(+1,+1,-1),float3(0, +1, 0), float2(0,1) },
			{ float3(+1,+1,+1),float3(0, +1, 0), float2(0,0) },
			{ float3(-1,+1,+1),float3(0, +1, 0), float2(1,0) },
			{ float3(-1,+1,-1),float3(0, +1, 0), float2(1,1) },

			{ float3(-1,+1,-1),float3(-1, 0, 0), float2(1,0) },
			{ float3(-1,+1,+1),float3(-1, 0, 0), float2(0,0) },
			{ float3(-1,-1,+1),float3(-1, 0, 0), float2(0,1) },
			{ float3(-1,-1,-1),float3(-1, 0, 0), float2(1,1) },

			{ float3(-1,-1,+1),float3(0, 0, +1), float2(1,1) },
			{ float3(+1,-1,+1),float3(0, 0, +1), float2(0,1) },
			{ float3(+1,+1,+1),float3(0, 0, +1), float2(0,0) },
			{ float3(-1,+1,+1),float3(0, 0, +1), float2(1,0) }
		};

		Utility::span<VertexAttr> sv(CubeVerts);
		// clang-format on
		for (size_t i = 0; i < sv.size(); i++)
		{
			sv[i].pos = (ext * 0.5).cwiseProduct(sv[i].pos);
		}
		BufferDesc VertBuffDesc;
		VertBuffDesc.Name = "Cube vertex buffer";
		VertBuffDesc.Usage = Diligent::USAGE::USAGE_IMMUTABLE;
		VertBuffDesc.BindFlags = Diligent::BIND_VERTEX_BUFFER;
		VertBuffDesc.Size = sv.size_bytes();
		BufferData VBData;
		VBData.pData = CubeVerts;
		VBData.DataSize = sizeof(CubeVerts);
		RefCntAutoPtr<IBuffer> pCubeVertexBuffer;
		pDevice->CreateBuffer(VertBuffDesc, &VBData, &pCubeVertexBuffer);

		return pCubeVertexBuffer;
	}
	template<>
	RefCntAutoPtr<IBuffer> Cubetemp<VLay::VAttribs1>::CreateVertexBuffer(IRenderDevice* pDevice, float3 ext)
	{
		// using namespace  Diligent;
		// Layout of this structure matches the one we defined in the pipeline state

		// Cube vertices

		//      (-1,+1,+1)________________(+1,+1,+1)
		//               /|              /|
		//              / |             / |
		//             /  |            /  |
		//            /   |           /   |
		//(-1,-1,+1) /____|__________/(+1,-1,+1)
		//           |    |__________|____|
		//           |   /(-1,+1,-1) |    /(+1,+1,-1)
		//           |  /            |   /
		//           | /             |  /
		//           |/              | /
		//           /_______________|/
		//        (-1,-1,-1)       (+1,-1,-1)
		//

		using VertexAttribs = std::tuple_element_t<Vai, geomatrys::vLayout>;
		// clang-format off
		VertexAttribs CubeVerts[] =
		{
			{ float3(-1,-1,-1),float3(0, 0, -1), float2(0,1) },
			{ float3(-1,+1,-1),float3(0, 0, -1), float2(0,0) },
			{ float3(+1,+1,-1),float3(0, 0, -1), float2(1,0) },
			{ float3(+1,-1,-1),float3(0, 0, -1), float2(1,1) },

			{ float3(-1,-1,-1),float3(0, -1, 0), float2(0,1) },
			{ float3(-1,-1,+1),float3(0, -1, 0), float2(0,0) },
			{ float3(+1,-1,+1),float3(0, -1, 0), float2(1,0) },
			{ float3(+1,-1,-1),float3(0, -1, 0), float2(1,1) },

			{ float3(+1,-1,-1),float3(+1, 0, 0), float2(0,1) },
			{ float3(+1,-1,+1),float3(+1, 0, 0), float2(1,1) },
			{ float3(+1,+1,+1),float3(+1, 0, 0), float2(1,0) },
			{ float3(+1,+1,-1),float3(+1, 0, 0), float2(0,0) },

			{ float3(+1,+1,-1),float3(0, +1, 0), float2(0,1) },
			{ float3(+1,+1,+1),float3(0, +1, 0), float2(0,0) },
			{ float3(-1,+1,+1),float3(0, +1, 0), float2(1,0) },
			{ float3(-1,+1,-1),float3(0, +1, 0), float2(1,1) },

			{ float3(-1,+1,-1),float3(-1, 0, 0), float2(1,0) },
			{ float3(-1,+1,+1),float3(-1, 0, 0), float2(0,0) },
			{ float3(-1,-1,+1),float3(-1, 0, 0), float2(0,1) },
			{ float3(-1,-1,-1),float3(-1, 0, 0), float2(1,1) },

			{ float3(-1,-1,+1),float3(0, 0, +1), float2(1,1) },
			{ float3(+1,-1,+1),float3(0, 0, +1), float2(0,1) },
			{ float3(+1,+1,+1),float3(0, 0, +1), float2(0,0) },
			{ float3(-1,+1,+1),float3(0, 0, +1), float2(1,0) }
		};

		Utility::span<VertexAttribs> sv(CubeVerts);
		// clang-format on
		for (size_t i = 0; i < sv.size(); i++)
		{
			sv[i].pos = (ext * 0.5).cwiseProduct(sv[i].pos);
		}
		BufferDesc VertBuffDesc;
		VertBuffDesc.Name = "Cube vertex buffer";
		VertBuffDesc.Usage = Diligent::USAGE::USAGE_IMMUTABLE;
		VertBuffDesc.BindFlags = Diligent::BIND_VERTEX_BUFFER;
		VertBuffDesc.Size = sv.size_bytes();
		BufferData VBData;
		VBData.pData = CubeVerts;
		VBData.DataSize = sizeof(CubeVerts);
		RefCntAutoPtr<IBuffer> pCubeVertexBuffer;
		pDevice->CreateBuffer(VertBuffDesc, &VBData, &pCubeVertexBuffer);

		return pCubeVertexBuffer;
	}
	static Collection coll;
	static std::array<StaticMesh,16> _smeshs;
	Utility::span<Diligent::LayoutElement> VertexLay(const int& i) {
		using namespace Diligent;
		static LayoutElement inputDesc0[] =
		{
			{ 0, 0, 3, VT_FLOAT32 },   //float3 Pos     : ATTRIB0;
			{ 1, 0, 3, VT_FLOAT32 },   //float3 Normal  : ATTRIB1;
			{ 2, 0, 2, VT_FLOAT32 },   //float2 UV0     : ATTRIB2;
			{ 3, 0, 2, VT_FLOAT32 },   //float2 UV1     : ATTRIB3;
			{ 4, 1, 4, VT_FLOAT32 },   //float3 Pos     : ATTRIB0;
			{ 5, 1, 4, VT_FLOAT32 }	   //float3 Normal  : ATTRIB1;
		};
		LayoutElement inputDesc1[] =
		{
		 {0, 0, 3, VT_FLOAT32},   //float3 Pos     : ATTRIB0;
		 {1, 0, 3, VT_FLOAT32},   //float3 Normal  : ATTRIB1;
		 {2, 0, 2, VT_FLOAT32},   //float2 UV0     : ATTRIB2;
		 {3, 0, 2, VT_FLOAT32},   //float2 UV1     : ATTRIB3;
		};
		LayoutElement inputDesc2[] =
		{
		 {0, 0, 3, VT_FLOAT32},   //float3 Pos     : ATTRIB0;
		 {1, 0, 3, VT_FLOAT32},   //float3 Normal  : ATTRIB1;
		 {2, 0, 2, VT_FLOAT32},   //float2 UV0     : ATTRIB2;
		};
		switch (i)
		{
		case 0:return inputDesc0;
		case 1:return inputDesc1;
		case 2:return inputDesc2;
		default:
			std::abort();
			return inputDesc0;
		}
	}
	///////////////////////////////////////////////////////////////////////////////
	// generate vertices for +X face only by intersecting 2 circular planes
	// (longitudinal and latitudinal) at the longitude/latitude angles
	///////////////////////////////////////////////////////////////////////////////
	std::vector<float> getUnitPositiveX(unsigned int pointsPerRow)
	{
		const float DEG2RAD = acos(-1) / 180.0f;

		std::vector<float> vertices;
		float n1[3];        // normal of longitudinal plane rotating along Y-axis
		float n2[3];        // normal of latitudinal plane rotating along Z-axis
		float v[3];         // direction vector intersecting 2 planes, n1 x n2
		float a1;           // longitudinal angle along y-axis
		float a2;           // latitudinal angle
		float scale;

		// rotate latitudinal plane from 45 to -45 degrees along Z-axis
		for (unsigned int i = 0; i < pointsPerRow; ++i)
		{
			// normal for latitudinal plane
			a2 = DEG2RAD * (45.0f - 90.0f * i / (pointsPerRow - 1));
			n2[0] = -sin(a2);
			n2[1] = cos(a2);
			n2[2] = 0;

			// rotate longitudinal plane from -45 to 45 along Y-axis
			for (unsigned int j = 0; j < pointsPerRow; ++j)
			{
				// normal for longitudinal plane
				a1 = DEG2RAD * (-45.0f + 90.0f * j / (pointsPerRow - 1));
				n1[0] = -sin(a1);
				n1[1] = 0;
				n1[2] = -cos(a1);

				// find direction vector of intersected line, n1 x n2
				v[0] = n1[1] * n2[2] - n1[2] * n2[1];
				v[1] = n1[2] * n2[0] - n1[0] * n2[2];
				v[2] = n1[0] * n2[1] - n1[1] * n2[0];


				vertices.push_back(v[0]);
				vertices.push_back(v[1]);
				vertices.push_back(v[2]);

				// DEBUG
				//std::cout << "vertex: (" << v[0] << ", " << v[1] << ", " << v[2] << "), "
				//          << sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]) << std::endl;
			}
		}

		return vertices;
	}
	void GenSph(RenderContext& rt,StaticMesh& re, float radius =1) {
		using va = DfVertexAttr;
		int stackCount = 10,sub=3;
		auto vertexCountPerRow = (unsigned int)pow(2, sub) + 1;
		auto vertexCountPerFace = vertexCountPerRow * vertexCountPerRow;
		// generate unit-length verties in +X face
		std::vector<float> unitVertices = getUnitPositiveX(vertexCountPerRow);

		std::vector<va> vertices;
		std::vector<uint> indices;
		std::vector<int> lineIndices;
		vertices.reserve(stackCount * stackCount);
		indices.reserve(vertices.size() * 3);
		lineIndices.reserve(vertices.size() * 3);

		auto addIndices = [&](const uint& a, const uint& b, const uint& c) {
			indices.emplace_back(a);
			indices.emplace_back(b);
			indices.emplace_back(c);
		};
		float x, y, z, s, t;
		uint k = 0, k1, k2;
		
		// build +X face
		for (unsigned int i = 0; i < vertexCountPerRow; ++i)
		{
			k1 = i * vertexCountPerRow;     // index for curr row
			k2 = k1 + vertexCountPerRow;    // index for next row
			t = (float)i / (vertexCountPerRow - 1);

			for (unsigned int j = 0; j < vertexCountPerRow; ++j, k += 3, ++k1, ++k2)
			{
				s = (float)j / (vertexCountPerRow - 1);
				auto& vt = vertices.emplace_back();
				std::memcpy(vt.pos.data(), unitVertices.data() + k, 4 * 3);
				vt.uv0 = { s,t };
				vt.normal = vt.pos;
				vt.pos *= radius;

				// add indices
				if (i < (vertexCountPerRow - 1) && j < (vertexCountPerRow - 1))
				{
					addIndices(k1, k2, k1 + 1);
					addIndices(k1 + 1, k2, k2 + 1);
					// lines: left and top
					lineIndices.push_back(k1);  // left
					lineIndices.push_back(k2);
					lineIndices.push_back(k1);  // top
					lineIndices.push_back(k1 + 1);
				}
			}
		}
		float3 dd(-1,1,-1);

		// array size and index for building next face
		unsigned int startIndex;                    // starting index for next face
		int vertexSize = (int)vertices.size();      // vertex array size of +X face
		int indexSize = (int)indices.size();        // index array size of +X face
		int lineIndexSize = (int)lineIndices.size(); // line index size of +X face

		// build -X face by negating x and z
		startIndex = vertices.size();// / 3;
		for (int i = 0, j = 0; i < vertexSize; i ++, j += 2)
		{
			auto& vt = vertices.emplace_back(vertices[i]);
			vt.pos=vt.pos.cwiseProduct(dd);
			vt.normal =vt.normal.cwiseProduct(dd);
		}
		for (int i = 0; i < indexSize; ++i)
		{
			indices.push_back(startIndex + indices[i]);
		}
		for (int i = 0; i < lineIndexSize; i += 4)
		{
			// left and bottom lines
			lineIndices.push_back(startIndex + lineIndices[i]);     // left
			lineIndices.push_back(startIndex + lineIndices[i + 1]);
			lineIndices.push_back(startIndex + lineIndices[i + 1]);  // bottom
			lineIndices.push_back(startIndex + lineIndices[i + 1] + 1);
		}

		// build +Y face by swapping x=>y, y=>-z, z=>-x
		startIndex = vertices.size();
		for (int i = 0, j = 0; i < vertexSize; i++, j += 2)
		{
			auto& sp = vertices[i].pos;
			auto& vt = vertices.emplace_back(vertices[i]);
			vt.pos = { -sp[2],sp[0],-sp[1]};
			vt.normal = vt.pos.normalized();
		}

		for (int i = 0; i < indexSize; ++i)
		{
			indices.push_back(startIndex + indices[i]);
		}
		for (int i = 0; i < lineIndexSize; ++i)
		{
			// top and left lines (same as +X)
			lineIndices.push_back(startIndex + lineIndices[i]);
		}

		// build -Y face by swapping x=>-y, y=>z, z=>-x
		startIndex = vertices.size();
		for (int i = 0, j = 0; i < vertexSize; i++, j += 2)
		{
			auto& sp = vertices[i].pos;
			auto& vt = vertices.emplace_back(vertices[i]);
			vt.pos = { -sp[2],sp[0],sp[1] };
			vt.normal = vt.pos.normalized();
		}
		for (int i = 0; i < indexSize; ++i)
		{
			indices.push_back(startIndex + indices[i]);
		}
		for (int i = 0; i < lineIndexSize; i += 4)
		{
			// top and right lines
			lineIndices.push_back(startIndex + lineIndices[i]); // top
			lineIndices.push_back(startIndex + lineIndices[i + 3]);
			lineIndices.push_back(startIndex + lineIndices[i] + 1); // right
			lineIndices.push_back(startIndex + lineIndices[i + 1] + 1);
		}

		// build +Z face by swapping x=>z, z=>-x
		startIndex = vertices.size();
		for (int i = 0, j = 0; i < vertexSize; i++, j += 2)
		{
			auto& sp = vertices[i].pos;
			auto& vt = vertices.emplace_back(vertices[i]);
			vt.pos = { -sp[2],sp[1],sp[0] };
			vt.normal = vt.pos.normalized();
		}
		for (int i = 0; i < indexSize; ++i)
		{
			indices.push_back(startIndex + indices[i]);
		}
		for (int i = 0; i < lineIndexSize; ++i)
		{
			// top and left lines (same as +X)
			lineIndices.push_back(startIndex + lineIndices[i]);
		}

		// build -Z face by swapping x=>-z, z=>x
		startIndex = vertices.size();
		for (int i = 0, j = 0; i < vertexSize; i++, j += 2)
		{
			auto& sp = vertices[i].pos;
			auto& vt = vertices.emplace_back(vertices[i]);
			vt.pos = { sp[2],sp[1],-sp[0] };
			vt.normal = vt.pos.normalized();
		}
		for (int i = 0; i < indexSize; ++i)
		{
			indices.push_back(startIndex + indices[i]);
		}
		for (int i = 0; i < lineIndexSize; i += 4)
		{
			// left and bottom lines
			lineIndices.push_back(startIndex + lineIndices[i]);     // left
			lineIndices.push_back(startIndex + lineIndices[i + 1]);
			lineIndices.push_back(startIndex + lineIndices[i + 1]);   // bottom
			lineIndices.push_back(startIndex + lineIndices[i + 1] + 1);
		}

		auto& pDevice = rt.m_pDevice;
		using namespace  Diligent;
		BufferDesc IndBuffDesc, VertBuffDesc;
		BufferData IBData,VBData;
		IndBuffDesc.Mode = BUFFER_MODE::BUFFER_MODE_STRUCTURED;
		VertBuffDesc=IndBuffDesc;
		const auto flag = BIND_FLAGS::BIND_INDEX_BUFFER | BIND_FLAGS::BIND_UNORDERED_ACCESS;
		{
			IBData.pData = indices.data();
			IBData.DataSize = Utility::VecByteSize(indices);
			IndBuffDesc.Name = "sphere index buffer";
			IndBuffDesc.Usage = Diligent::USAGE::USAGE_IMMUTABLE;
			IndBuffDesc.BindFlags = flag;
			IndBuffDesc.Size = IBData.DataSize;
			IndBuffDesc.ElementByteStride = sizeof(int);
			RefCntAutoPtr<IBuffer>& pBuffer=re.IndexBuffer;
			pDevice->CreateBuffer(IndBuffDesc, &IBData, &pBuffer); 
		}
		{
			VertBuffDesc.Name = "sphere vertex buffer";
			VertBuffDesc.Usage = Diligent::USAGE::USAGE_IMMUTABLE;
			VertBuffDesc.BindFlags = flag;
			VertBuffDesc.Size = Utility::VecByteSize(vertices);
			VertBuffDesc.ElementByteStride = sizeof(va);
			VBData.pData = vertices.data();
			VBData.DataSize = VertBuffDesc.Size;
			RefCntAutoPtr<IBuffer>& pBuffer = re.VertexBuffer;
			pDevice->CreateBuffer(VertBuffDesc, &VBData, &pBuffer);
		}
	}

	void Init(RenderContext& rt) {
		using namespace Diligent;
		coll.Init(rt);
		std::vector<StateTransitionDesc> decc_;
		decc_.reserve(32);
		auto transstate = [&](auto& geo) {
			decc_.emplace_back(geo.m_CubeIndexBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE);
			decc_.emplace_back(geo.m_CubeVertexBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE);
		};
		transstate(coll.uCube0);
		transstate(coll.uCube1);
		GenSph(rt, _smeshs[0]);
		rt.m_pImmediateContext->TransitionResourceStates(decc_.size(), decc_.data());
		constexpr uint k = 56 << 24;
		constexpr uint k1 = k >> 24;
		/*unitCube.Init(rt.m_pDevice);
		uCube1.Init(rt.m_pDevice);*/
	}
	Collection& GetColl() { return coll; }
	StaticMesh& GetMesh(const int& i) {
		return _smeshs[i];
	}
}