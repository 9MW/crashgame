#pragma once
#include "Common/interface/RefCntAutoPtr.hpp"
#include <Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h>

#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"
#include <ShaderMacroHelper.hpp>
#include"Common/span.hpp"
#include"Common/Utility.h"
#include"Graphics/GraphicsTools/interface/CommonlyUsedStates.h"
#include"Graphics/GraphicsTools/interface/GraphicsUtilities.h"

namespace pj2 {
	using math::uint;
	namespace Imp {
		using namespace Diligent;
		struct RenderContext
		{
			//using namespace Diligent;
			Diligent::TEXTURE_FORMAT RenderTargetFormat = Diligent::TEX_FORMAT_RGBA8_UNORM;
			Diligent::TEXTURE_FORMAT DepthBufferFormat = Diligent::TEX_FORMAT_D32_FLOAT;
			Diligent::RefCntAutoPtr<Diligent::IRenderDevice> m_pDevice;
			Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;
			Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> pShaderSourceFactory;
			Diligent::RefCntAutoPtr<Diligent::IBuffer> pCameraAttribs;
			Diligent::RefCntAutoPtr<Diligent::ISwapChain> m_pSwapChain;
			RefCntAutoPtr<Diligent::ITextureView>           m_RendeSRV;
			RefCntAutoPtr<Diligent::ITextureView> m_pDepthDSV;

		};
	}
	using Imp::RenderContext;
	const std::string rendertag[] = { "test","red1" ,"red2" };
	namespace DrawFunc {

		using namespace Diligent;
		void CreateComputePSO(
			const char* EntryPoint,
			const char* FilePath,
			const char* Name,
			RefCntAutoPtr<IRenderDevice> m_pDevice,
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory,
			RefCntAutoPtr<IPipelineState>& p_pso,
			nonstd::span<ShaderResourceVariableDesc>Vars,
			int m_ThreadGroupSize = -1, ShaderMacroHelper Macros = {});

		void CreateComputePSO(
			const char* EntryPoint,
			const char* FilePath,
			const char* Name,
			RefCntAutoPtr<IRenderDevice> m_pDevice,
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory,
			RefCntAutoPtr<IPipelineState>& p_pso,
			int m_ThreadGroupSize = -1);

		void CreateSetBUf(RefCntAutoPtr<IRenderDevice>& m_pDevice,
			RefCntAutoPtr<IDeviceContext>& _pImmediateContext,
			const BufferDesc BuffDesc, IBuffer** ppBuffer,
			RefCntAutoPtr<IShaderResourceBinding>& psrb, Diligent::SHADER_TYPE ST = Diligent::SHADER_TYPE_COMPUTE, const BufferData* pBuffData = nullptr);

		void SetBUf(RefCntAutoPtr<IRenderDevice>& m_pDevice,
			RefCntAutoPtr<IDeviceContext>& _pImmediateContext,
			const BufferDesc BuffDesc, IBuffer** ppBuffer,
			RefCntAutoPtr<IShaderResourceBinding>& psrb, Diligent::SHADER_TYPE ST = Diligent::SHADER_TYPE_COMPUTE, const BufferData* pBuffData = nullptr);
		void CreateSetBUf(RefCntAutoPtr<IRenderDevice>& m_pDevice,
			RefCntAutoPtr<IDeviceContext>& _pImmediateContext, const BufferDesc BuffDesc,
			IBuffer** ppBuffer, RefCntAutoPtr<IShaderResourceBinding>& psrb, Diligent::SHADER_TYPE ST,
			const BufferData* pBuffData = nullptr, Diligent::BIND_FLAGS bf = Diligent::BIND_NONE);

	};


	struct alignas(16) IndirectArgs {
		math::uint32_t NumIndices,
			NumInstances,
			FirstIndexLocation,
			BaseVertex,
			FirstInstanceLocation;
	};
	struct AnimRec {
		math::uint bonenum, base, size, modelId;
		std::string name;
		AnimRec() :bonenum(0), base(0), size(0) {}
	};
	namespace n_Render {
		using IndirectArgs = IndirectArgs;
		using namespace Diligent;
		template<class T>
		using list = std::vector<T>;
		enum TEX_ID
		{
			// Base color for metallic-roughness workflow or
			// diffuse color for specular-glossinees workflow
			TEXTURE_ID_BASE_COLOR = 0,

			// Metallic-roughness or specular-glossinees map
			TEXTURE_ID_PHYSICAL_DESC,

			TEXTURE_ID_NORMAL_MAP,
			TEXTURE_ID_OCCLUSION,
			TEXTURE_ID_EMISSIVE,
			TEXTURE_ID_NUM_TEXTURES
		};
		template<uint srb = 1>
		struct PSB
		{
			RefCntAutoPtr<IPipelineState> PSO;
			//RefCntAutoPtr<IShaderResourceBinding> SRB;
			std::array<RefCntAutoPtr<IShaderResourceBinding>, srb> SRBs;
			void CreateShaderResourceBinding() {
				for (size_t i = 0; i < srb; i++)
				{
					auto& cptr = SRBs[i];
					PSO->CreateShaderResourceBinding(&cptr, true);
				}
			}

			constexpr auto& GetShaderResourceBinding(uint i=0) {
				return SRBs[i];
			}
		};
		struct Render {
			static inline int AnimSamplerate = 60;
			void mergetest() {
				math::Efloat4 data(0.2, 0.3, 0.6, 0.67);
				Uint32 color = 0, c22;
				std::byte col[4];
				for (size_t i = 0; i < 4; i++)
				{
					color |= uint(math::Clamp01((float)data[i]) * 255) << i * 8;
					col[i] = (std::byte)(math::Clamp01((float)data[i]) * 255);
				}
				c22 = *reinterpret_cast<uint*>(col);
			}


			virtual int WriteVData(void* pda) {
				return -1;
			}
			virtual int Draw(RenderContext& rt, Diligent::DrawIndexedAttribs& da) {
				return -1;
			}
			static inline RefCntAutoPtr<ITextureView> m_pWhiteTexSRV;
			static inline RefCntAutoPtr<ITextureView> m_pBlackTexSRV;
			static inline RefCntAutoPtr<ITextureView> m_pDefaultNormalMapSRV;
			static inline RefCntAutoPtr<ITexture> m_pWhiteTex;
			static inline RefCntAutoPtr<ITexture> m_pBlackTex;
			static inline RefCntAutoPtr<ITexture> m_pDefaultNormalMap;

			//auto SetTexture = [&](GLTF::Material::TEXTURE_ID TexId, ITextureView* pDefaultTexSRV, const char* VarName) //
			//{
			//	RefCntAutoPtr<ITextureView> pTexSRV;

			//	auto TexIdx = Material.TextureIds[TexId];
			//	if (TexIdx >= 0)
			//	{
			//		if (auto* pTexture = Model.GetTexture(TexIdx))
			//		{
			//			if (pTexture->GetDesc().Type == RESOURCE_DIM_TEX_2D_ARRAY)
			//				pTexSRV = pTexture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
			//			else
			//			{
			//				TextureViewDesc SRVDesc;
			//				SRVDesc.ViewType = TEXTURE_VIEW_SHADER_RESOURCE;
			//				SRVDesc.TextureDim = RESOURCE_DIM_TEX_2D_ARRAY;
			//				pTexture->CreateView(SRVDesc, &pTexSRV);
			//			}
			//		}
			//	}

			//	if (pTexSRV == nullptr)
			//		pTexSRV = pDefaultTexSRV;

			//	if (auto* pVar = pSRB->GetVariableByName(SHADER_TYPE_PIXEL, VarName))
			//		pVar->Set(pTexSRV);
			//};
			static void CreateTex(
				RefCntAutoPtr<IRenderDevice> m_pDevice,
				RefCntAutoPtr<IDeviceContext> m_pImmediateContext,
				ITexture* pTex,
				const float* data
			) {

				static constexpr Uint32 TexDim = 8;
				TextureDesc TexDesc;
				TexDesc.Name = "White texture for GLTF renderer";
				TexDesc.Type = RESOURCE_DIM_TEX_2D;
				TexDesc.Usage = Diligent::USAGE::USAGE_IMMUTABLE;
				TexDesc.BindFlags = BIND_SHADER_RESOURCE;
				TexDesc.Width = TexDim;
				TexDesc.Height = TexDim;
				TexDesc.Format = TEX_FORMAT_RGBA8_UNORM;
				TexDesc.MipLevels = 1;
				UINT32 color = 0;
				for (size_t i = 0; i < 4; i++)
				{
					uint ff = (uint)(std::clamp(data[i], 0.0f, 1.0f) * 255);
					color |= ff << i * 8;
				}
				std::vector<Uint32>     Data(TexDim * TexDim, color);
				TextureSubResData       Level0Data{ Data.data(), TexDim * 4 };
				TextureData             InitData{ &Level0Data, 1 };
				m_pDevice->CreateTexture(TexDesc, &InitData, &pTex);
			}

			static void UpdateWholeTex(IDeviceContext* Ctx, ITexture& TargtTex, TextureSubResData& SubresData) {
				Diligent::Box UpdateBox;
				UpdateBox.MinX = 0;
				UpdateBox.MinY = 0;
				UpdateBox.MaxX = TargtTex.GetDesc().Width;
				UpdateBox.MaxY = TargtTex.GetDesc().Height;
				Ctx->UpdateTexture(&TargtTex, 0, 0, UpdateBox, SubresData, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			}
			static void CreateintTex(
				RefCntAutoPtr<IRenderDevice> m_pDevice,
				RefCntAutoPtr<IDeviceContext> m_pImmediateContext,
				ITexture*& pTex,
				const void* data,
				Uint32 h = 8,
				Uint32 w = 8,
				const std::string texName = "texture for renderer"
			) {
				if (data == NULL) {
					pTex = NULL;
					return;
				}
				TextureDesc TexDesc;
				TexDesc.Name = texName.c_str();
				TexDesc.Type = RESOURCE_DIM_TEX_2D;
				TexDesc.Usage = Diligent::USAGE::USAGE_DEFAULT;
				TexDesc.BindFlags = BIND_SHADER_RESOURCE;
				TexDesc.Width = w;
				TexDesc.Height = h;
				TexDesc.Format = TEX_FORMAT_R32_SINT;
				TexDesc.MipLevels = 1;
				UINT32 color = 0;
				TextureSubResData       Level0Data{ data, TexDesc.Width * 4 };
				TextureData             InitData{ &Level0Data, 1 };
				m_pDevice->CreateTexture(TexDesc, &InitData, &pTex);
			}
			void Init(
				RefCntAutoPtr<IRenderDevice> m_pDevice,
				RefCntAutoPtr<IDeviceContext> m_pImmediateContext
			) {
				using namespace Diligent;
				if (!m_pWhiteTexSRV) {
					{
						static constexpr Uint32 TexDim = 8;
						TextureDesc TexDesc;
						TexDesc.Name = "White texture for GLTF renderer";
						TexDesc.Type = RESOURCE_DIM_TEX_2D;
						TexDesc.Usage = Diligent::USAGE::USAGE_IMMUTABLE;
						TexDesc.BindFlags = BIND_SHADER_RESOURCE;
						TexDesc.Width = TexDim;
						TexDesc.Height = TexDim;
						TexDesc.Format = TEX_FORMAT_RGBA8_UNORM;
						TexDesc.MipLevels = 1;
						std::vector<Uint32>     Data(TexDim * TexDim, 0xFFFFFFFF);
						TextureSubResData       Level0Data{ Data.data(), TexDim * 4 };
						TextureData             InitData{ &Level0Data, 1 };
						RefCntAutoPtr<ITexture>& pWhiteTex=m_pWhiteTex;
						m_pDevice->CreateTexture(TexDesc, &InitData, &pWhiteTex);
						m_pWhiteTexSRV = pWhiteTex->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);

						TexDesc.Name = "Black texture for GLTF renderer";
						for (auto& c : Data) c = 0;
						RefCntAutoPtr<ITexture>& pBlackTex=m_pBlackTex;
						m_pDevice->CreateTexture(TexDesc, &InitData, &pBlackTex);
						m_pBlackTexSRV = pBlackTex->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);

						TexDesc.Name = "Default normal map for GLTF renderer";
						for (auto& c : Data) c = 0x00FF7F7F;
						RefCntAutoPtr<ITexture>& pDefaultNormalMap=m_pDefaultNormalMap;
						m_pDevice->CreateTexture(TexDesc, &InitData, &pDefaultNormalMap);
						m_pDefaultNormalMapSRV = pDefaultNormalMap->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);

						// clang-format off
						StateTransitionDesc Barriers[] =
						{
							{pWhiteTex,         RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE},
							{pBlackTex,         RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE},
							{pDefaultNormalMap, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE}
						};
						// clang-format on
						m_pImmediateContext->TransitionResourceStates(_countof(Barriers), Barriers);
						RefCntAutoPtr<ISampler> pDefaultSampler;
						m_pDevice->CreateSampler(Sam_LinearClamp, &pDefaultSampler);
						m_pWhiteTexSRV->SetSampler(pDefaultSampler);
						m_pBlackTexSRV->SetSampler(pDefaultSampler);
						m_pDefaultNormalMapSRV->SetSampler(pDefaultSampler);
					}
				}
			}
		};

		struct dyvbf
		{
			using ui64 = math::ui64;
			struct mergeresult
			{
				bool reAlloc = false, success = false;
			};
			dyvbf() {
				std::memset(this, 0, sizeof(*this));
			}
			inline const ui64& size() {
				return _size;
			}
			void reserve(RenderContext& rt, BufferDesc& VBDesc) {
				const auto& reserve = VBDesc.Size;
				if (m_pObject) {
					m_pObject->Release();
				}
				cap = reserve;
				auto& pDevice = rt.m_pDevice;
				pDevice->CreateBuffer(VBDesc, NULL, &m_pObject);
			}
			void merge(RefCntAutoPtr<IBuffer>& vdatain, RenderContext& rt, int expandfactor = 2);
			mergeresult mergel(const void* pdata, const ui64& sz, RenderContext& rt) {
				const auto& vd = m_pObject->GetDesc();
				auto& pDevice = rt.m_pDevice;
				const auto stbef = m_pObject->GetState();
				mergeresult mr;
				if (sz + _size < cap) {
					rt.m_pImmediateContext->UpdateBuffer(m_pObject, _size, sz, pdata, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
				}
				else
				{
					std::abort();
				}
				
				_size += sz;
				return mr;
			}
			template<class T1>
			inline void append(const T1& da, RenderContext& rt) {
				constexpr int ali = sizeof(T1);
				const auto stbef = m_pObject->GetState();
				if ((_size + 1) < cap) {
					rt.m_pImmediateContext->UpdateBuffer(m_pObject, _size*ali, ali, &da, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
				}
				else
				{
					std::abort();
				}
				_size++;
			}
			template<class T1>
			inline void updater(const T1& da, RenderContext& rt, const ui64& i) {
				assert(i < cap);
				constexpr int ali = sizeof(T1);
				const auto stbef= m_pObject->GetState();
				rt.m_pImmediateContext->UpdateBuffer(m_pObject, i* ali, ali, &da, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
				rt.m_pImmediateContext->TransitionResourceStates(1, 
					new(&sstd)StateTransitionDesc(m_pObject, RESOURCE_STATE_UNKNOWN, stbef, STATE_TRANSITION_FLAG_UPDATE_STATE));
			}
			template<class T1>
			inline void updateat(const T1& da, RenderContext& rt, const ui64& byteoff) {
				constexpr int ali = sizeof(T1);
				const auto stbef = m_pObject->GetState();
				assert(byteoff / ali < cap);
				rt.m_pImmediateContext->UpdateBuffer(m_pObject, byteoff, ali, &da, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
				rt.m_pImmediateContext->TransitionResourceStates(1,
					new(&sstd)StateTransitionDesc(m_pObject, RESOURCE_STATE_UNKNOWN, stbef, STATE_TRANSITION_FLAG_UPDATE_STATE));
			}
			template<class T1>
			inline void updateat(const Utility::span<T1> da, RenderContext& rt, const ui64& byteoff) {
				constexpr int ali = sizeof(T1);
				const auto stbef = m_pObject->GetState();
				assert((byteoff / ali+da.size()) < cap);
				rt.m_pImmediateContext->UpdateBuffer(m_pObject, byteoff, da.size_bytes(), da.data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
				rt.m_pImmediateContext->TransitionResourceStates(1,
					new(&sstd)StateTransitionDesc(m_pObject, RESOURCE_STATE_UNKNOWN, stbef, STATE_TRANSITION_FLAG_UPDATE_STATE));
			}
			template<class T1>
			mergeresult merge(std::vector<T1> datas, RenderContext& rt) {
				return mergel(datas.data(), datas.size() * sizeof(T1), rt);
			}
			template<class T1>
			mergeresult merge(Utility::span<T1> datas, RenderContext& rt) {
				return mergel(datas.data(), datas.size() * sizeof(T1), rt);
			}
			using T = IBuffer;
			T* RawPtr() noexcept { return m_pObject; }
			const T* RawPtr() const noexcept { return m_pObject; }


			T* operator->() noexcept { return m_pObject; }
			const T* operator->() const noexcept { return m_pObject; }
			operator T* () noexcept { return RawPtr(); }
			operator const T* () const noexcept { return RawPtr(); }
			void Set(RefCntAutoPtr<IBuffer>& pin){
				if (m_pObject) {
					m_pObject->Release();
				}
				_size = pin->GetDesc().Size;
				cap = _size;
				m_pObject = pin;
			}
			dyvbf& operator=(RefCntAutoPtr<IBuffer>& pin)
			{ 
				assert(!m_pObject);
				_size = pin->GetDesc().Size;
				cap = _size;
				m_pObject=pin;
				return *this;
			}
			void reset() {
				Release();
				new(this)dyvbf();
			}
			void resize() {
				Release();
				new(this)dyvbf();
			}
			void  Release() {
				if (m_pObject) {
					m_pObject->Release();
				}
				std::memset(this, 0, sizeof(*this));
			}
			~dyvbf() {
				Release();
			}
		protected:
			static inline StateTransitionDesc sstd;
			ui64 cap, _size;
			IBuffer* m_pObject;
		};
	}
	namespace RenderUtilImp {

		using Ichar = char;
		using namespace Diligent;
		static  ImmutableSamplerDesc defStaticSamplers[] =
		{
			{SHADER_TYPE_PIXEL, "g_ColorMap", Diligent::Sam_LinearWrap}
		};
		void CreatePipelineStates(IPipelineState** const pRenderMeshPSO,
			RefCntAutoPtr<IRenderDevice> m_pDevice, Diligent::TEXTURE_FORMAT ColorBufferFormat,
			Diligent::TEXTURE_FORMAT DepthBufferFormat,
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory,
			const std::string FilePath, const Ichar* ven, const Ichar* pen,
			const Utility::span<ShaderResourceVariableDesc> sd,
			const Utility::span<LayoutElement> inputLayoutDesc,
			const Utility::span<ImmutableSamplerDesc> StaticSamplers = defStaticSamplers,
			PRIMITIVE_TOPOLOGY VertexTop = PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			bool AntialiasedLineEnable = false, bool DepthEnable = true,
			const Ichar* vnm = "dfvert", const Ichar* pnm = "dffrag",
			const Diligent::CULL_MODE cullmode = CULL_MODE_BACK,
			ShaderMacroHelper Macros = {});
		inline void CreatePipelineStates(IPipelineState** const pRenderMeshPSO,
			RefCntAutoPtr<IRenderDevice> m_pDevice, Diligent::TEXTURE_FORMAT ColorBufferFormat,
			Diligent::TEXTURE_FORMAT DepthBufferFormat,
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory,
			const std::string FilePath, const Ichar* ven, const Ichar* pen,
			const Utility::span<ShaderResourceVariableDesc> sd,
			const ShaderMacroHelper Macros,
			const Utility::span<LayoutElement> inputLayoutDesc,
			const Utility::span<ImmutableSamplerDesc> StaticSamplers = defStaticSamplers,
			PRIMITIVE_TOPOLOGY VertexTop = PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			bool AntialiasedLineEnable = false, bool DepthEnable = true,
			const Ichar* vnm = "dfvert", const Ichar* pnm = "dffrag",
			const Diligent::CULL_MODE cullmode = CULL_MODE_BACK
		) {
			CreatePipelineStates(pRenderMeshPSO,
				m_pDevice, ColorBufferFormat,
				DepthBufferFormat,
				pShaderSourceFactory,
				FilePath, ven, pen,
				sd,
				inputLayoutDesc,
				StaticSamplers,
				VertexTop,
				AntialiasedLineEnable, DepthEnable,
				vnm, pnm,
				cullmode,
				Macros);
		}
		template<class T>
		inline void UpdateBuffer(
			Diligent::RefCntAutoPtr<Diligent::IDeviceContext>m_pImmediateContext,
			IBuffer* pBuffer,
			Uint32                         offset,
			std::vector<T>& VfArgspan,
			RESOURCE_STATE_TRANSITION_MODE StateTransitionMode
		)
		{

			m_pImmediateContext->UpdateBuffer(pBuffer, offset, VfArgspan.size() * sizeof(T), VfArgspan.data(), Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		}
		void CreateSetStaticBUf(RefCntAutoPtr<IRenderDevice>& m_pDevice,
			RefCntAutoPtr<IDeviceContext>& _pImmediateContext, const BufferDesc BuffDesc,
			IBuffer** ppBuffer, RefCntAutoPtr<IPipelineState>& pso, Diligent::SHADER_TYPE ST,
			const BufferData* pBuffData = nullptr, Diligent::BIND_FLAGS bf = Diligent::BIND_NONE);
		using BIND_FLAGS = Diligent::BIND_FLAGS;
		template<class T>
		void VbufferCreate(IRenderDevice* pDevice,
			std::vector<T>& VfArgspan,
			const Char* Name,
			IBuffer** ppBuffer,
			USAGE Usage                     DEFAULT_VALUE(USAGE_DYNAMIC),
			Diligent::BIND_FLAGS BindFlags            DEFAULT_VALUE(BIND_UNIFORM_BUFFER),
			Diligent::CPU_ACCESS_FLAGS CPUAccessFlags DEFAULT_VALUE(CPU_ACCESS_WRITE)) {
			CreateUniformBuffer(pDevice, Utility::VecByteSize(VfArgspan), Name,
				ppBuffer, Usage, BindFlags, CPUAccessFlags, VfArgspan.data());
		}
		template<class T>
		void BuildBDesc(BufferDesc& CBDesc,
			Uint64           Size,
			const Char* Name,
			const BUFFER_MODE mode,
			USAGE            Usage,
			BIND_FLAGS       BindFlags,
			CPU_ACCESS_FLAGS CPUAccessFlags DEFAULT_VALUE(CPU_ACCESS_WRITE)) {

			CBDesc.Name = Name;
			CBDesc.Usage = Usage;
			CBDesc.BindFlags = BindFlags;
			CBDesc.CPUAccessFlags = CPUAccessFlags;
			CBDesc.Mode = mode;
			CBDesc.ElementByteStride = sizeof(T);
			CBDesc.Size = Size * CBDesc.ElementByteStride;
		}



		//void VbufferCreate(IRenderDevice* pDevice,
		//	std::vector<T>& VfArgspan,
		//	const Char* Name,
		//	IBuffer** ppBuffer,
		//	USAGE Usage                     DEFAULT_VALUE(USAGE_DYNAMIC),
		//	BIND_FLAGS BindFlags            DEFAULT_VALUE(BIND_UNIFORM_BUFFER),
		//	CPU_ACCESS_FLAGS CPUAccessFlags DEFAULT_VALUE(CPU_ACCESS_WRITE)) {
		//	CreateUniformBuffer(pDevice, Utility::VecSize(VfArgspan), Name,
		//		ppBuffer, Usage, BindFlags, CPUAccessFlags, VfArgspan.data());
		//}
	}
	using ui64 = math::ui64;
	namespace RenderUtil {
		using RenderUtilImp::Ichar;
		using RenderUtilImp::UpdateBuffer;
		using  Diligent::IShaderSourceInputStreamFactory;
		using  Diligent::BufferData;
		using  Diligent::IRenderDevice;
		using  Diligent::IDeviceContext;
		using  Diligent::IShaderResourceBinding;
		using  Diligent::IBuffer;
		using  Diligent::IPipelineState;
		using  Diligent::RefCntAutoPtr;
		using ui32 = uint32_t;
		using  pj2::DrawFunc::CreateSetBUf;
		using  pj2::DrawFunc::SetBUf;
		using RenderUtilImp::CreatePipelineStates;
		using RenderUtilImp::VbufferCreate;
		using RenderUtilImp::BuildBDesc;
		using Diligent::CreateUniformBuffer;
		//constexpr auto VbufferCreate = Diligent::CreateUniformBuffer;

		void CreateCompPSOStr(const char* EntryPoint, const char* FilePath, const char* Name, RefCntAutoPtr<IRenderDevice> m_pDevice, RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory, RefCntAutoPtr<IPipelineState>& p_pso,
			Utility::span<Diligent::ShaderResourceVariableDesc> Vars, int m_ThreadGroupSize);

		inline void setVbuffer(
			IShaderResourceBinding* psrb,
			RefCntAutoPtr<IPipelineState>& p_pso,
			Utility::span<Diligent::ShaderResourceVariableDesc> srv,
			Utility::span<Diligent::BUFFER_VIEW_TYPE> bview = {},
			Utility::span<IBuffer*> vbuf = {}) {
			using namespace Diligent;
			for (size_t i = 0; i < vbuf.size(); i++)
			{
				Diligent::ShaderResourceVariableDesc& sd = srv[i];
				IBuffer*& buf = vbuf[i];
				if(sd.Type== SHADER_RESOURCE_VARIABLE_TYPE_STATIC){
					if (bview[i] == Diligent::BUFFER_VIEW_UNDEFINED) {
						p_pso->GetStaticVariableByName(sd.ShaderStages, sd.Name)->Set(buf);
					}
					else
					{
						p_pso->GetStaticVariableByName(sd.ShaderStages, sd.Name)->Set(buf->GetDefaultView(bview[i]));
					}
				}
				else
				{
					psrb->GetVariableByName(sd.ShaderStages,sd.Name)->Set(buf->GetDefaultView(bview[i]));
				}
			}
		}
		void CCPSO(
			const char* EntryPoint,
			const char* FilePath,
			const char* Name,
			RefCntAutoPtr<IRenderDevice> m_pDevice,
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory,
			RefCntAutoPtr<IPipelineState>& p_pso,
			Utility::span<Diligent::ShaderResourceVariableDesc> Vars,
			Diligent::ShaderMacroHelper Macros,
			int m_ThreadGroupSize = -1);
		/*void CreatePipelineStates(
			IPipelineState** const pRenderMeshPSO,
			RefCntAutoPtr<IRenderDevice> m_pDevice,
			Diligent::TEXTURE_FORMAT ColorBufferFormat,
			Diligent::TEXTURE_FORMAT DepthBufferFormat,
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory,
			const std::string FilePath,
			const Ichar* ven,
			const Ichar* pen,
			const Utility::span<Diligent::ShaderResourceVariableDesc> sd,
			const Utility::span<Diligent::LayoutElement> inputLayoutDesc,
			const Utility::span<Diligent::ImmutableSamplerDesc> StaticSamplers = {
				{Diligent::SHADER_TYPE_PIXEL, "g_ColorMap", Diligent::Sam_LinearWrap}
			},
			Diligent::PRIMITIVE_TOPOLOGY VertexTop = Diligent::PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			bool AntialiasedLineEnable = false,
			bool DepthEnable = true,
			const Ichar* vnm = NULL,
			const Ichar* pnm = NULL) {
			RenderUtilImp::CreatePipelineStates(pRenderMeshPSO,
				m_pDevice,
				ColorBufferFormat,
				DepthBufferFormat,
				pShaderSourceFactory,
				FilePath,
				ven,pen,
				StaticSamplers, {}, {}, VertexTop,false, true
				)
		}*/
		using Ichar = char;
		template<int srb = 1>
		struct PSB
		{
			RefCntAutoPtr<IPipelineState> PSO;
			//RefCntAutoPtr<IShaderResourceBinding> SRB;
			std::array<RefCntAutoPtr<IShaderResourceBinding>, srb> SRBs;
			void CreateShaderResourceBinding() {
				for (size_t i = 0; i < srb; i++)
				{
					auto& cptr = SRBs[i];
					PSO->CreateShaderResourceBinding(&cptr, true);
				}
			}
			void Release() {
				PSO = NULL;
				for (size_t i = 0; i < SRBs.size(); i++)
				{
					SRBs[i] = NULL;
				}
			}
			constexpr auto& GetShaderResourceBinding(const int i = 0) {
				return SRBs[i];
			}
		};
		struct PSBDY
		{
			RefCntAutoPtr<IPipelineState> PSO;
			//RefCntAutoPtr<IShaderResourceBinding> SRB;
			std::vector<RefCntAutoPtr<IShaderResourceBinding>> SRBs;
			void CreateShaderResourceBinding(int n) {
				SRBs.resize(n);
				for (size_t i = 0; i < n; i++)
				{
					auto& cptr = SRBs[i];
					PSO->CreateShaderResourceBinding(&cptr, true);
				}
			}
			void Release() {
				PSO = NULL;
				for (size_t i = 0; i < SRBs.size(); i++)
				{
					SRBs[i] = NULL;
				}
			}
			inline auto& GetShaderResourceBinding(const int i = 0) {
				return SRBs[i];
			}
		};
		inline void md_BufferDesc
		(
			const char* name, Diligent::BufferDesc& BuffDesc, ui32 uiSize, ui32 ElementByteStride,
			Diligent::USAGE Usage, Diligent::BIND_FLAGS BindFlags, Diligent::CPU_ACCESS_FLAGS CPUAccessFlags
		) {
			BuffDesc.Name = name;
			BuffDesc.Size = uiSize * ElementByteStride;// sizeof(sStruct::GlobalConstants);// 24byte round to 32 byte
			BuffDesc.ElementByteStride = ElementByteStride;// sizeof(sStruct::GlobalConstants);// 24byte round to 32 byte
			BuffDesc.Usage = Usage;
			BuffDesc.BindFlags = BindFlags;
			BuffDesc.CPUAccessFlags = CPUAccessFlags;
			//m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_brightcb);
		}
		class UtilRender
		{
		public:
			UtilRender() {

			}
			void Init(RenderContext& rt) {
				_rt = rt;
			}
		private:
			RenderContext _rt;
		};
		//void CreatePipelineStates(
		   // IPipelineState** const pRenderMeshPSO,
		   // RefCntAutoPtr<IRenderDevice> m_pDevice,
		   // Diligent::TEXTURE_FORMAT ColorBufferFormat,
		   // Diligent::TEXTURE_FORMAT DepthBufferFormat,
		   // RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory,
		   // const std::string FilePath,
		   // const Ichar* ven,
		   // const Ichar* pen,
		   // const	Utility::span<ShaderResourceVariableDesc> sd={},
		   // const	Utility::span<LayoutElement>inputLayoutDesc = {},
		   // const	Utility::span<ImmutableSamplerDesc> StaticSamplers = {}
		   // /* PRIMITIVE_TOPOLOGY VertexTop = PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST*/
		//) {}
		//template<int srb = 1>
		//struct PSB
		//{
		   // RefCntAutoPtr<IPipelineState> PSO;
		   // //RefCntAutoPtr<IShaderResourceBinding> SRB;
		   // std::array<RefCntAutoPtr<IShaderResourceBinding>, srb> SRBs;
		   // void CreateShaderResourceBinding() {
			  //  for (size_t i = 0; i < srb; i++)
			  //  {
				 //   auto& cptr = SRBs[i];
				 //   PSO->CreateShaderResourceBinding(&cptr, true);
			  //  }
		   // }

		   // void CreateShaderResourceBinding(RefCntAutoPtr<IPipelineState>& PSO_) {
			  //  PSO = PSO_;
			  //  for (size_t i = 0; i < srb; i++)
			  //  {
				 //   auto& cptr = SRBs[i];
				 //   PSO->CreateShaderResourceBinding(&cptr, true);
			  //  }
		   // }
		   // constexpr auto& GetShaderResourceBinding(uint i) {
			  //  return SRBs[i];
		   // }
		//};
	}
	namespace ReUtS {

		template<int srb = 1>
		using PSB = RenderUtil::PSB<srb>;
		using t_psb1 = PSB<1>;
	}
};