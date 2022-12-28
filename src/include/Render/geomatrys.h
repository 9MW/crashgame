#pragma once
#include"Common/Math1.h"
#include<BasicMath.hpp>
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Common/interface/RefCntAutoPtr.hpp"
#include"Render/RenderUtil.h"
namespace pj2::geomatrys {
	using IBuffer = Diligent::IBuffer;
	using IRenderDevice = Diligent::IRenderDevice;
	using IDeviceContext = Diligent::IDeviceContext;
	using Diligent::RefCntAutoPtr;// = Diligent::IDeviceContext;
	struct GeoVertexAttribs0
	{
		using float3 = math::Vector3f;
		using float2 = math::Vector2f;
		float3 pos;
		float3 normal;
		float2 uv0;
		float2 uv1;
	};
	struct GeoVertexAttribs1
	{
		using float3 = math::Vector3f;
		using float2 = math::Vector2f;
		float3 pos;
		float3 normal;
		float2 uv0;
	};
	namespace Laydescs{
		using namespace Diligent;
		static LayoutElement defDesc[] =
		{
		 {0, 0, 3, VT_FLOAT32},   //float3 Pos     : ATTRIB0;
		 {1, 0, 3, VT_FLOAT32},   //float3 Normal  : ATTRIB1;
		 {2, 0, 2, VT_FLOAT32},   //float2 UV0     : ATTRIB2;
		 {3, 0, 2, VT_FLOAT32},   //float2 UV1     : ATTRIB3;
		};
	}
	Utility::span<Diligent::LayoutElement> VertexLay(const int& i);
	struct ngeo_quads {
		RefCntAutoPtr<IBuffer> pVertexBuffer;
		RefCntAutoPtr<IBuffer> m_IndexBuffer;
		static constexpr math::uint IndexCount = 6;
		ngeo_quads(IRenderDevice* pDevice, Diligent::float3 scale = Diligent::float3(1, 1, 1)) {
			m_IndexBuffer = CreateIndexBuffer(pDevice);
			CreateVertexBuffer(pDevice, scale);
		}
		ngeo_quads() {  }
		void init(IRenderDevice* pDevice, Diligent::float3 scale = Diligent::float3(1, 1, 1)) {

			m_IndexBuffer = CreateIndexBuffer(pDevice);
			CreateVertexBuffer(pDevice, scale);
		}
		void TransitionResource(IDeviceContext* const Dc) {
			using namespace  Diligent;
			StateTransitionDesc Barriers[] =
			{
				{pVertexBuffer,         RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_VERTEX_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE},
				{m_IndexBuffer,         RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_INDEX_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE}
			};
			Dc->TransitionResourceStates(_countof(Barriers), Barriers);
		}
		void CreateVertexBuffer(IRenderDevice* pDevice, Diligent::float3 scale)
		{
			using namespace  Diligent;
			using float3 = Diligent::float3;
			using float2 = Diligent::float2;
			using float4 = Diligent::float4;
			// Layout of this structure matches the one we defined in the pipeline state
			struct VertexAttribs0
			{
				float3 pos;
				float3 normal;
				float2 uv0;
				float2 uv1;
			};
			struct VertexAttribs1
			{
				float4 joint0;
				float4 weight0;
			};
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
			VertexAttribs0 CubeVerts[] =
			{
				//pos normal uv
				{float3(-1,-1,0),float3(0, 0, -1), float2(0,0)},
				{float3(-1,+1,0),float3(0, 0, -1), float2(0,1)},
				{float3(+1,+1,0),float3(0, 0, -1), float2(1,1)},
				{float3(+1,-1,0),float3(0, 0, -1), float2(1,0)}
			};
			nonstd::span cv(CubeVerts);
			for (size_t i = 0; i < cv.size(); i++)
			{
				cv[i].pos = cv[i].pos * scale * 0.5f;
			}
			//VertexAttribs1 v1[_countof(CubeVerts)];
			//// clang-format on
			//CreateUniformBuffer(pDevice, sizeof(v1), "vb1", &pVertexBuffer[1], Diligent::USAGE::USAGE_IMMUTABLE, BIND_VERTEX_BUFFER,
			//	CPU_ACCESS_NONE, v1);
			BufferDesc VertBuffDesc;
			VertBuffDesc.Name = "quads vertex buffer";
			VertBuffDesc.Usage = Diligent::USAGE::USAGE_IMMUTABLE;
			VertBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
			VertBuffDesc.Size = sizeof(CubeVerts);
			BufferData VBData;
			VBData.pData = CubeVerts;
			VBData.DataSize = sizeof(CubeVerts);
			RefCntAutoPtr<IBuffer> pCubeVertexBuffer;

			pDevice->CreateBuffer(VertBuffDesc, &VBData, &pVertexBuffer);

		}
		// clang-format o

		RefCntAutoPtr<IBuffer> CreateIndexBuffer(IRenderDevice* pDevice)
		{
			using namespace  Diligent;
			// clang-format off
			Uint32 Indices[] =
			{
				1,0,2,    0,3,2
			};
			// clang-format on

			BufferDesc IndBuffDesc;
			IndBuffDesc.Name = "Cube index buffer";
			IndBuffDesc.Usage = Diligent::USAGE::USAGE_IMMUTABLE;
			IndBuffDesc.BindFlags = BIND_INDEX_BUFFER;
			IndBuffDesc.Size = sizeof(Indices);
			BufferData IBData;
			IBData.pData = Indices;
			IBData.DataSize = sizeof(Indices);
			RefCntAutoPtr<IBuffer> pBuffer;
			pDevice->CreateBuffer(IndBuffDesc, &IBData, &pBuffer);
			return pBuffer;
		}
	};
	enum VLay
	{
		VAttribs0, VAttribs1
	};
	using vLayout = std::tuple<GeoVertexAttribs0, GeoVertexAttribs1>;
	//using namespace Diligent;
	template<VLay Va>
	struct Cubetemp {
	private:
		static constexpr int Vai = (int)Va;
		using float3 = math::Efloat3;
		using float2 = math::Efloat2;
	public:
		static RefCntAutoPtr<IBuffer> CreateVertexBuffer(IRenderDevice* pDevice, float3 ext = { 1,1,1 });
		// clang-format o
		static constexpr UINT32 Indices[] =
		{
			2,0,1,    2,3,0,
			4,6,5,    4,7,6,
			8,10,9,   8,11,10,
			12,14,13, 12,15,14,
			16,18,17, 16,19,18,
			20,21,22, 20,22,23
		};
		static RefCntAutoPtr<IBuffer> CreateIndexBuffer(IRenderDevice* pDevice)
		{
			using namespace  Diligent;
			BufferDesc IndBuffDesc;
			IndBuffDesc.Name = "Cubetemp index buffer";
			IndBuffDesc.Usage = Diligent::USAGE::USAGE_IMMUTABLE;
			IndBuffDesc.BindFlags = BIND_INDEX_BUFFER;
			IndBuffDesc.Size = sizeof(Indices);
			BufferData IBData;
			IBData.pData = Indices;
			IBData.DataSize = sizeof(Indices);
			RefCntAutoPtr<IBuffer> pBuffer;
			pDevice->CreateBuffer(IndBuffDesc, &IBData, &pBuffer);
			return pBuffer;
		}
		RefCntAutoPtr<IBuffer>                m_CubeVertexBuffer;
		RefCntAutoPtr<IBuffer>                m_CubeIndexBuffer;
		Cubetemp(IRenderDevice* pDevice, const float3& ext = {1,1,1}) {
			Init(pDevice, ext);
		}
		Cubetemp(){}
		void Init(IRenderDevice* pDevice, const float3& ext = { 1,1,1 }) {
			m_CubeVertexBuffer = NULL;
			m_CubeIndexBuffer = NULL;
			m_CubeVertexBuffer = CreateVertexBuffer(pDevice, ext);
			m_CubeIndexBuffer = CreateIndexBuffer(pDevice);
		}
		~Cubetemp() {

		}
	};
	struct StaticMesh
	{
		RefCntAutoPtr<IBuffer>VertexBuffer,IndexBuffer;
		int indexnum = 0;
	};
	/*template<class T>
	struct Cubeup {};
	template<class ...Ts>
	struct Cubeup<std::tuple<Ts...>> {
		using C = std::tuple<Cubetemp<VLay::VAttribs0>...>;
	}*/
	using Cube = Cubetemp<VLay::VAttribs0>;
	static Cube unitCube;
	static Cubetemp<VLay::VAttribs1> uCube1;
	struct Collection {
		Cube uCube0;
		Cubetemp<VLay::VAttribs1> uCube1; 
		void Init(RenderContext& rt) {
			uCube0.Init(rt.m_pDevice);
			uCube1.Init(rt.m_pDevice);
		}

	};
	using Utility::circle;
	void Init(RenderContext& rt);
	Collection& GetColl();

	StaticMesh& GetMesh(const int& i);
}