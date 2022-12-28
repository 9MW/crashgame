#pragma once
#include<PCH.h>
#include "Common/interface/RefCntAutoPtr.hpp"
#include <Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h>

#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"
#include<CommonlyUsedStates.h>
#include <ShaderMacroHelper.hpp>

#include"Image.h"
#include "MapHelper.hpp"
#include "RenderUtil.h"
#include "GraphicsUtilities.h"
#include "TextureUtilities.h"
#include"BasicMath.hpp"
#include "FileSystem.hpp"
#include "FileWrapper.hpp"
#include <DataBlobImpl.hpp>
#include"Common/Math1.h"
#include"Common/span.hpp"
#include"Log/log.h"
#include <imgui.h>
namespace pj2 {
	struct instance
	{
		int animid, transformid, materialid;
		float dt;
	};
	struct Color32 {

		static constexpr float black[] = { 1, 1, 1, 1 };
		static constexpr float white[] = { 0, 0, 0, 1 };
	};
	namespace GIns {
		using mtransfrom = math::mtransform;
		using namespace nonstd;
		using namespace Diligent;
		class StreamingBuffer
		{
		public:
			StreamingBuffer(IRenderDevice* pDevice, Diligent::BIND_FLAGS BindFlags, Uint32 Size, size_t NumContexts, const Char* Name) :
				m_BufferSize{ Size },
				m_MapInfo(NumContexts)
			{
				BufferDesc BuffDesc;
				BuffDesc.Name = Name;
				BuffDesc.Usage = USAGE_DYNAMIC;
				BuffDesc.BindFlags = BindFlags;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
				BuffDesc.Size = Size;
				pDevice->CreateBuffer(BuffDesc, nullptr, &m_pBuffer);
			}

			// Returns offset of the allocated region
			Uint32 Allocate(IDeviceContext* pCtx, Uint32 Size, size_t CtxNum)
			{
				auto& MapInfo = m_MapInfo[CtxNum];
				// Check if there is enough space in the buffer
				if (MapInfo.m_CurrOffset + Size > m_BufferSize)
				{
					// Unmap the buffer
					Flush(CtxNum);
				}

				if (MapInfo.m_MappedData == nullptr)
				{
					// If current offset is zero, we are mapping the buffer for the first time after it has been flushed. Use MAP_FLAG_DISCARD flag.
					// Otherwise use MAP_FLAG_NO_OVERWRITE flag.
					MapInfo.m_MappedData.Map(pCtx, m_pBuffer, MAP_WRITE, MapInfo.m_CurrOffset == 0 ? MAP_FLAG_DISCARD : MAP_FLAG_NO_OVERWRITE);
				}

				auto Offset = MapInfo.m_CurrOffset;
				// Update offset
				MapInfo.m_CurrOffset += Size;
				return Offset;
			}

			void Release(size_t CtxNum)
			{
				if (!m_AllowPersistentMap)
				{
					m_MapInfo[CtxNum].m_MappedData.Unmap();
				}
			}

			void Flush(size_t CtxNum)
			{
				m_MapInfo[CtxNum].m_MappedData.Unmap();
				m_MapInfo[CtxNum].m_CurrOffset = 0;
			}

			IBuffer* GetBuffer() { return m_pBuffer; }
			void* GetMappedCPUAddress(size_t CtxNum)
			{
				return m_MapInfo[CtxNum].m_MappedData;
			}

			void AllowPersistentMapping(bool AllowMapping)
			{
				m_AllowPersistentMap = AllowMapping;
			}

		private:
			RefCntAutoPtr<IBuffer> m_pBuffer;
			const Uint32           m_BufferSize;

			bool m_AllowPersistentMap = false;

			struct MapInfo
			{
				MapHelper<Uint8> m_MappedData;
				Uint32           m_CurrOffset = 0;
			};
			// We need to keep track of mapped data for every context
			std::vector<MapInfo> m_MapInfo;
		};

		static LayoutElement defDesc[] =
		{
		 {0, 0, 3, VT_FLOAT32},   //float3 Pos     : ATTRIB0;
		 {1, 0, 3, VT_FLOAT32},   //float3 Normal  : ATTRIB1;
		 {2, 0, 2, VT_FLOAT32},   //float2 UV0     : ATTRIB2;
		 {3, 0, 2, VT_FLOAT32},   //float2 UV1     : ATTRIB3;
		};

		static LayoutElement defLineDesc[] =
		{
		 {0, 0, 3, VT_FLOAT32},   //float3 Pos     : ATTRIB0;
		 {1, 0, 3, VT_FLOAT32},   //float3 Normal  : ATTRIB1;
		 {2, 0, 2, VT_FLOAT32},   //float2 UV0     : ATTRIB2;
		 {3, 0, 2, VT_FLOAT32},   //float2 UV1     : ATTRIB3;
		};
		using ImmutableSamplerDesc = Diligent::ImmutableSamplerDesc;
		static ImmutableSamplerDesc defStaticSamplers[] =
		{
			{SHADER_TYPE_PIXEL, "g_ColorMap", Diligent::Sam_LinearWrap}
		};
		/*template<class T>
		using span = Utility::span<T>;*/
		using Ichar = char;
		static void CreatePipelineStates(
			IPipelineState** const pRenderMeshPSO,
			RefCntAutoPtr<IRenderDevice> m_pDevice,
			GraphicsPipelineStateCreateInfo PSOCreateInfo,
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory,
			const std::string FilePath,
			const Ichar* ven,
			const Ichar* pen,
			const	span<ShaderResourceVariableDesc> sd,
			const	span<LayoutElement>inputDesc = defDesc,
			const	span<ImmutableSamplerDesc> StaticSamplers = defStaticSamplers,
			PRIMITIVE_TOPOLOGY VertexTop = PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			bool AntialiasedLineEnable = false,
			bool DepthEnable = true) {
			ShaderCreateInfo                               ShaderCI;
			ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;

			ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;
			ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
			ShaderCI.UseCombinedTextureSamplers = true;

			ShaderMacroHelper Macros;
			// clang-format on
			ShaderCI.Macros = Macros;

			ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
			ShaderCI.Desc.Name = FilePath.c_str();
			ShaderCI.EntryPoint = ven;
			ShaderCI.FilePath = FilePath.c_str();
			RefCntAutoPtr<IShader> pVS;
			m_pDevice->CreateShader(ShaderCI, &pVS);

			ShaderCI.Desc.Name = FilePath.c_str();
			ShaderCI.EntryPoint = pen;
			ShaderCI.FilePath = FilePath.c_str();
			ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
			RefCntAutoPtr<IShader> pPS;
			m_pDevice->CreateShader(ShaderCI, &pPS);
			//for (Uint32 vb = 0; vb < m_Mesh._subMesh.size(); ++vb)
			{
				Diligent::GraphicsPipelineDesc& GraphicsPipeline = PSOCreateInfo.GraphicsPipeline;
				PipelineStateDesc& PSODesc = PSOCreateInfo.PSODesc;
				PSOCreateInfo.pVS = pVS;
				PSOCreateInfo.pPS = pPS;
				m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, pRenderMeshPSO);
			}
		}

		template <typename OutputIt>
		class fixed_iterator {
		private:
			typedef std::iterator_traits<OutputIt> traits;

			OutputIt out_;
			std::size_t limit_;
			std::size_t count_;

		public:
			typedef std::output_iterator_tag iterator_category;
			typedef typename traits::value_type value_type;
			typedef typename traits::difference_type difference_type;
			typedef typename traits::pointer pointer;
			typedef typename traits::reference reference;

			fixed_iterator(OutputIt out, std::size_t limit)
				: out_(out), limit_(limit), count_(0) {}

			OutputIt base() const { return out_; }
			std::size_t count() const { return count_; }

			fixed_iterator& operator++() {
				if (count_++ < limit_)
					++out_;
				return *this;
			}

			fixed_iterator operator++(int) {
				auto it = *this;
				++* this;
				return it;
			}

			reference operator*() const {
				if (count_ >= limit_)
					throw std::runtime_error("end of buffer");
				return *out_;
			}
		};
		using RenderUtil::CreatePipelineStates;
		inline void CreatePipelineStates(
			IPipelineState ** const pRenderMeshPSO,
			RefCntAutoPtr<IRenderDevice> m_pDevice,
			RefCntAutoPtr<ISwapChain> m_pSwapChain,
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory,
			const std::string FilePath,
			const Ichar* ven,
			const Ichar* pen,
			const	span<ShaderResourceVariableDesc> sd, 
			const Ichar* vnm = NULL,
			const Ichar* pnm = NULL,
			const	span<LayoutElement>inputDesc = defDesc,
			const	span<ImmutableSamplerDesc> StaticSamplers = defStaticSamplers,
			PRIMITIVE_TOPOLOGY VertexTop = PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			bool AntialiasedLineEnable = false,
			bool DepthEnable = true)
		{
			CreatePipelineStates(
				 pRenderMeshPSO,
				m_pDevice,
				m_pSwapChain->GetDesc().ColorBufferFormat,
				m_pSwapChain->GetDesc().DepthBufferFormat,
				pShaderSourceFactory,
				FilePath,
				ven,
				pen,
				sd,
				inputDesc,
				StaticSamplers,
				VertexTop,
				AntialiasedLineEnable,
				DepthEnable,vnm,pnm);
		}
		static void CreatePipelineStates(
			IPipelineState** const pRenderMeshPSO,
			RefCntAutoPtr<IRenderDevice> m_pDevice,
			Diligent::TEXTURE_FORMAT ColorBufferFormat,
			Diligent::TEXTURE_FORMAT DepthBufferFormat,
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory,
			const std::string FilePath,
			const Ichar* ven,
			const Ichar* pen,
			const	span<ShaderResourceVariableDesc> sd)
		{

			const	span<LayoutElement>inputDesc = defDesc;
				const	span<ImmutableSamplerDesc> StaticSamplers = defStaticSamplers;
				PRIMITIVE_TOPOLOGY VertexTop = PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
				bool AntialiasedLineEnable = false;
				bool DepthEnable = true;
				const Ichar* vnm = NULL;
				const Ichar* pnm = NULL;
			CreatePipelineStates(
				pRenderMeshPSO,
				m_pDevice,
				ColorBufferFormat,
				DepthBufferFormat,
				pShaderSourceFactory,
				FilePath,
				ven,
				pen,
				sd,
				inputDesc,
				StaticSamplers,
				VertexTop,
				AntialiasedLineEnable,
				DepthEnable, vnm, pnm);
		}
		/*inline void CreatePipelineStates(
			IPipelineState** const pRenderMeshPSO,
			RefCntAutoPtr<IRenderDevice> m_pDevice,
			Diligent::TEXTURE_FORMAT ColorBufferFormat,
			Diligent::TEXTURE_FORMAT DepthBufferFormat,
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory,
			const std::string FilePath,
			const Ichar* ven,
			const Ichar* pen,
			const	span<ShaderResourceVariableDesc> sd,
			const	span<LayoutElement>inputDesc = defDesc,
			const	span<ImmutableSamplerDesc> StaticSamplers = defStaticSamplers,
			PRIMITIVE_TOPOLOGY VertexTop = PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			bool AntialiasedLineEnable = false,
			bool DepthEnable = true)
		{
				CreatePipelineStates(
					pRenderMeshPSO,
					m_pDevice,
					ColorBufferFormat,
					DepthBufferFormat,
					pShaderSourceFactory,
					FilePath,
					ven,
					pen,
					sd,
					inputDesc,
					StaticSamplers,
					VertexTop,
					AntialiasedLineEnable,
					DepthEnable);
		}*/
		inline void CreatePipelineStates(
			IPipelineState*& pRenderMeshPSO,
			RefCntAutoPtr<IRenderDevice>   m_pDevice,
			RefCntAutoPtr<ISwapChain>      m_pSwapChain,
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory,
			const std::string FilePath,
			const	span<ShaderResourceVariableDesc> sd,
			const	span<LayoutElement>inputDesc = defDesc,
			const	span< ImmutableSamplerDesc> StaticSamplers = defStaticSamplers,
			PRIMITIVE_TOPOLOGY VertexTop = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			bool AntialiasedLineEnable = false) {
			CreatePipelineStates(
				&pRenderMeshPSO,
				m_pDevice,
				m_pSwapChain,
				pShaderSourceFactory,
				FilePath,
				"vert", "frag",
				sd,NULL,NULL, inputDesc, StaticSamplers, VertexTop, AntialiasedLineEnable
			);
		}
		struct renderCtx
		{
			RefCntAutoPtr<IRenderDevice> m_pDevice;
			RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
			RefCntAutoPtr<ISwapChain> m_pSwapChain;
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
		};
		template<class T>
		void CreateBuffer(RefCntAutoPtr<IRenderDevice>& m_pDevice,
			RefCntAutoPtr<IBuffer>& buf,
			int instancenum = 1) {
			BufferDesc BuffDesc;
			BuffDesc.Name = "CreateBuffer buffer";
			BuffDesc.Usage = USAGE_DEFAULT;
			BuffDesc.BindFlags = BIND_UNIFORM_BUFFER;
			BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
			BuffDesc.ElementByteStride = sizeof(T);
			BuffDesc.Size = sizeof(T) * instancenum;
			m_pDevice->CreateBuffer(BuffDesc, nullptr, &buf);
		}
		struct CalMapId
		{
			int sq;
			CalMapId(int sqsize) :sq(sqsize) {}
			CalMapId() {}
			uint CalIndex(int x, int y) {
				x += sq / 2;
				y += sq / 2;
				return x * sq + y;
			}
		};

	}
}