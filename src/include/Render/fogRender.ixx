#pragma once

#include <FirstPersonCamera.hpp>
#include"Render/ShaderStructs.h"
#include"io/loader.h"
#include"Common/span.hpp"
#include"Render/ModuleHead.h"
#include"RenderUtil.h"
#include"AssetLoader/RenderStr0.h"
//#include "ThirdParty/GLTFLoader.hpp";
#include"parallel_hashmap/phmap.h"
#include"cfgparser.h"
#include"Gameprops0.h"
#include <Render/RenderCommon.h>
#include"Render/geomatrys.h"
#include"structs.h"
//#include"Math1.h"
export module fogRender;

namespace pj2::n_Render {
	export class FowbatRender
	{
		static constexpr int deltaChunks = (1024 * 32);
		static constexpr int deltaObjs = deltaChunks / sizeof(sStruct::objrep);
		static constexpr int m_ThreadGroupSize = 64;
		static constexpr int QueueSize = 1024 * 128;
		template<class T>
		using span = nonstd::span<T>;
		RefCntAutoPtr<IBuffer>                m_CamConstants;
		RefCntAutoPtr<IBuffer>                m_deltaObjs;
		RefCntAutoPtr<IBuffer>                m_brightObjs;
		RefCntAutoPtr<IBuffer>                m_deltaObjsL;
		RefCntAutoPtr<IBuffer>                m_brightcb;
		RefCntAutoPtr<IBuffer>                queueFB;
		RefCntAutoPtr<IBuffer>                m_Moveprop;
		geomatrys::ngeo_quads plane;
		void init2(
			RefCntAutoPtr<IRenderDevice>& m_pDevice) {

		}
	public:
		FowbatRender() {}
		//FowbatRender(
		//	RefCntAutoPtr<IRenderDevice> m_pDevice,
		//	RefCntAutoPtr<IDeviceContext> m_pImmediateContext,
		//	RefCntAutoPtr<ISwapChain> pSwapChain,
		//	//IBuffer* pCameraAttribs,
		//	RefCntAutoPtr<IBuffer>                m_VertexBuffer,
		//	RefCntAutoPtr<IBuffer>                m_IndexBuffer,
		//	RefCntAutoPtr<IEngineFactory>              pEngineFactory) :m_pSwapChain(pSwapChain) {
		//	auto shaderpath = loader::Get(shader);
		//	init(m_pDevice, m_pImmediateContext, pSwapChain, pEngineFactory);
		//}
		void ClearBlock(int num = 1000) {
			auto& pso = m_pResetPSO;
			{
				Diligent::MapHelper<sStruct::GlobalConstants> gconst(m_pImmediateContext, m_brightcb, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
				*gconst = gCons;
			}
			m_pImmediateContext->SetPipelineState(pso);
			m_pImmediateContext->CommitShaderResources(m_pReset1SRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			//[numthreads(8, 8, 1)] 4x4 pixel perthread
			DispatAttribs.ThreadGroupCountX = shadowmapsz / 32;
			DispatAttribs.ThreadGroupCountY = shadowmapsz / 32;
			m_pImmediateContext->DispatchCompute(DispatAttribs);
		}
		void CreateRenderTargetPSO(
			IPipelineState** m_pRTPSO,
			RefCntAutoPtr<IRenderDevice> m_pDevice,
			RefCntAutoPtr<ISwapChain> pSwapChain,
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory,
			const std::string FilePath,
			const char* ven,
			const char* pen)
		{
			GraphicsPipelineStateCreateInfo RTPSOCreateInfo;

			// Pipeline state name is used by the engine to report issues
			// It is always a good idea to give objects descriptive names
			// clang-format off
			RTPSOCreateInfo.PSODesc.Name = "Render Target PSO";
			// This is a graphics pipeline
			RTPSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
			// This tutorial will render to a single render target
			RTPSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
			// Set render target format which is the format of the swap chain's color buffer
			RTPSOCreateInfo.GraphicsPipeline.RTVFormats[0] = m_pSwapChain->GetDesc().ColorBufferFormat;
			// Set depth buffer format which is the format of the swap chain's back buffer
			RTPSOCreateInfo.GraphicsPipeline.DSVFormat = m_pSwapChain->GetDesc().DepthBufferFormat;
			// Primitive topology defines what kind of primitives will be rendered by this pipeline state
			RTPSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
			// Cull back faces
			RTPSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_BACK;
			// Enable depth testing
			RTPSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = False;
			// clang-format on

			ShaderCreateInfo ShaderCI;
			// Tell the system that the shader source code is in HLSL.
			// For OpenGL, the engine will convert this into GLSL under the hood.
			ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

			// OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
			ShaderCI.UseCombinedTextureSamplers = true;

			ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;

			// Create a vertex shader
			RefCntAutoPtr<IShader> pRTVS;
			{
				ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
				ShaderCI.Desc.Name = FilePath.c_str();
				ShaderCI.EntryPoint = ven;
				ShaderCI.FilePath = FilePath.c_str();
				m_pDevice->CreateShader(ShaderCI, &pRTVS);
			}


#if PLATFORM_ANDROID
			// Vulkan on mobile platforms may require handling surface pre-transforms
			const bool TransformUVCoords = m_pDevice->GetDeviceCaps().IsVulkanDevice();
#else
			constexpr bool TransformUVCoords = false;
#endif

			ShaderMacroHelper Macros;
			Macros.AddShaderMacro("TRANSFORM_UV", TransformUVCoords);
			ShaderCI.Macros = Macros;

			// Create a pixel shader
			RefCntAutoPtr<IShader> pRTPS;
			{
				ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
				ShaderCI.Desc.Name = FilePath.c_str();
				ShaderCI.EntryPoint = pen;
				ShaderCI.FilePath = FilePath.c_str();
				m_pDevice->CreateShader(ShaderCI, &pRTPS);

				//// Create dynamic uniform buffer that will store our transformation matrix
				//// Dynamic buffers can be frequently updated by the CPU
				//BufferDesc CBDesc;
				//CBDesc.Name = "RTPS constants CB";
				//CBDesc.Size = sizeof(float4) + sizeof(float2x2) * 2;
				//CBDesc.Usage = USAGE_DYNAMIC;
				//CBDesc.BindFlags = BIND_UNIFORM_BUFFER;
				//CBDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
				//m_pDevice->CreateBuffer(CBDesc, nullptr, &m_RTPSConstants);
			}

			RTPSOCreateInfo.pVS = pRTVS;
			RTPSOCreateInfo.pPS = pRTPS;

			// Define variable type that will be used by default
			RTPSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

			// clang-format off
			// Shader variables should typically be mutable, which means they are expected
			// to change on a per-instance basis
			ShaderResourceVariableDesc Vars[] =
			{
				{ SHADER_TYPE_PIXEL, "fowresult", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
				{ SHADER_TYPE_PIXEL, "lightMapfow", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC }
			};
			// clang-format on
			RTPSOCreateInfo.PSODesc.ResourceLayout.Variables = Vars;
			RTPSOCreateInfo.PSODesc.ResourceLayout.NumVariables = _countof(Vars);

			// clang-format off
			// Define immutable sampler for g_Texture. Immutable samplers should be used whenever possible
			ImmutableSamplerDesc ImtblSamplers[] =
			{
				{ SHADER_TYPE_PIXEL, "fowresult", Sam_LinearClamp }
			};
			// clang-format on
			RTPSOCreateInfo.PSODesc.ResourceLayout.ImmutableSamplers = ImtblSamplers;
			RTPSOCreateInfo.PSODesc.ResourceLayout.NumImmutableSamplers = _countof(ImtblSamplers);

			m_pDevice->CreateGraphicsPipelineState(RTPSOCreateInfo, m_pRTPSO);

			// Since we did not explcitly specify the type for Constants, default type
			// (SHADER_RESOURCE_VARIABLE_TYPE_STATIC) will be used. Static variables never change and are bound directly
			// to the pipeline state object.
		}
		RefCntAutoPtr<IBuffer>                m_VFArg;
		RefCntAutoPtr<IBuffer>                m_VFArgR;
		RefCntAutoPtr<IBuffer>                m_VFArglocal;
		RefCntAutoPtr<IBuffer>                m_Vresi;
		RefCntAutoPtr<IBuffer>                m_vfProps;
		RefCntAutoPtr<IBuffer>                m_vfTrans;
		RefCntAutoPtr<IBuffer>                m_soBFinfo;
		//RefCntAutoPtr<IBuffer>                _rdBFinfo;

		//std::vector<IDeviceObject*> _bfviews;
		int VfArgREle = 512,_bufCount;
		//span<sStruct::VFoutArgs> dspan = {};
		void UpdateVFoutArgs(span<sStruct::VFoutArgs> VfArgspan,
			RefCntAutoPtr<IDeviceContext> m_pImmediateContext,
			Uint32 offset = 0) {
			m_pImmediateContext->UpdateBuffer(m_VFArg, offset, VfArgspan.size_bytes(), VfArgspan.data(), Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		}
		
		void addlitbuf(RenderAttr& para) {

		}
		void init(
			RenderContext& rc,
			//Utility::span<RenderAttr> buffers,
			ITextureView* tview,
			
			Utility::span<sStruct::bufferinfo> bufinfo,
			Utility::span<sStruct::VFoutArgs> VfArgspan = {},
			size_t Maxinstancnum = 16384)
		{
			RefCntAutoPtr<IBuffer>                m_Shadowqueue;
			RefCntAutoPtr<IBuffer>                m_RWSign;
			RefCntAutoPtr<IBufferView> pRWSignUAV;
			RefCntAutoPtr<IBufferView> pRWBrightUAV;
			RefCntAutoPtr<ITexture> pRTColor, pRTColorfow;
			auto& m_pDevice = rc.m_pDevice;
			auto& m_pImmediateContext = rc.m_pImmediateContext;
			auto& pSwapChain = rc.m_pSwapChain;
			this->m_pSwapChain = pSwapChain;
			this->m_pImmediateContext = m_pImmediateContext;
			auto& pShaderSourceFactory=rc.pShaderSourceFactory;
			auto shaderpath = loader::Get(shader);
			const auto& bufnum = bufinfo.size();
			_bufCount = bufnum;
			RefCntAutoPtr<IBuffer>                ConstantsL;
			{

				{
					ShaderResourceVariableDesc Vars[] =
					{
						//{SHADER_TYPE_COMPUTE, "queueFB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
						{SHADER_TYPE_COMPUTE, "Constants", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
						{SHADER_TYPE_COMPUTE, "ConstantsL", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
						{SHADER_TYPE_COMPUTE, "lightMap", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
						{ SHADER_TYPE_COMPUTE, "clearlightMap", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
						{SHADER_TYPE_VERTEX, "Cbuf", SHADER_RESOURCE_VARIABLE_TYPE_STATIC}
					};
					ShaderResourceVariableDesc Varspixel[] =
					{
						//{SHADER_TYPE_PIXEL, "lightMapfow", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
						{SHADER_TYPE_PIXEL, "lightMapfow", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
						{SHADER_TYPE_PIXEL, "resistanceMapR", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
						{SHADER_TYPE_VERTEX, "Cbuf", SHADER_RESOURCE_VARIABLE_TYPE_STATIC}
					};
					SamplerDesc SamLinearClampDesc
					{
						FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
						TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
					};
					SamplerDesc SampointClampDesc
					{
						FILTER_TYPE_POINT, FILTER_TYPE_POINT, FILTER_TYPE_POINT,
						TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
					};
					ImmutableSamplerDesc ImtblSamplers[] =
					{
						{SHADER_TYPE_PIXEL, "fowresult", SampointClampDesc},
						{SHADER_TYPE_PIXEL, "lightMapfow", SamLinearClampDesc}
					};
					ShaderMacroHelper sm;
					sm.AddShaderMacro("NUM_BUf", _bufCount);
					RenderUtil::CCPSO("Vfilter", "ShadowCast.hlsl", "sCull", m_pDevice,
						pShaderSourceFactory, Vfilterpsb.PSO, Vars, sm);
					//m_RfowPSO->Release();
					CreateRenderTargetPSO(&m_pRTPSO, m_pDevice, m_pSwapChain, pShaderSourceFactory, "fov.hlsl", "fowv1", "fowf1");
					//CreateRenderTargetPSO(&m_RfowPSO, m_pDevice, m_pSwapChain, pShaderSourceFactory, "fov.hlsl", "fowv", "fowf");
					/*const auto RenderTargetFormat = m_pSwapChain->GetDesc().ColorBufferFormat;
					const auto DepthBufferFormat = m_pSwapChain->GetDesc().DepthBufferFormat;*/
					RenderUtil::CreatePipelineStates
					(&m_RfowPSO, m_pDevice, RenderTargetFormat, DepthBufferFormat, pShaderSourceFactory,
						"fov.hlsl", "fowv", "fowf", Varspixel, GIns::defDesc, ImtblSamplers, Diligent::PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
						false, false);
					/*GIns::CreatePipelineStates(&m_RfowPSO, m_pDevice, m_pSwapChain,
						pShaderSourceFactory, "fov.hlsl", "fowv", "fowf", Varspixel, "fowv", "fowf", GIns::defDesc, ImtblSamplers);*/
						//PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,false,false);

					DrawFunc::CreateComputePSO("move", "ShadowCast.hlsl", "move", m_pDevice, pShaderSourceFactory, m_pMovePSO, Vars);
					DrawFunc::CreateComputePSO("fov", "CastMain.hlsl", "fov", m_pDevice, pShaderSourceFactory, m_ClightPSO, Vars);
					//Vars[0] = { SHADER_TYPE_COMPUTE, "clearlightMap", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC };
					DrawFunc::CreateComputePSO("reset", "ShadowCast.hlsl", "reset", m_pDevice, pShaderSourceFactory, m_pResetPSO, Vars);
				}

				BufferData VBData;

				plane.init(m_pDevice, Diligent::float3(shadowmapsz, shadowmapsz, shadowmapsz));
				plane.TransitionResource(m_pImmediateContext);
				BufferDesc BuffDesc;

				BuffDesc.Name = "m_brightObjs CB";
				BuffDesc.Size = 128;// sizeof(sStruct::GlobalConstants);// 24byte round to 32 byte
				BuffDesc.Usage = USAGE_DYNAMIC;
				BuffDesc.BindFlags = BIND_UNIFORM_BUFFER;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
				m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_brightcb);

				/*BuffDesc.Name = "RTPS constants CB";
				BuffDesc.Size = sizeof(sStruct::CameraAttribs);
				BuffDesc.Usage = USAGE_DYNAMIC;
				BuffDesc.BindFlags = BIND_UNIFORM_BUFFER;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
				m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_CamConstants);*/

				CreateUniformBuffer(m_pDevice, 256 * 2, "Camera attribs buffer", &m_CamConstants);

				BuffDesc.Name = "m_brightObjs";
				BuffDesc.ElementByteStride = sizeof(sStruct::objrep);
				BuffDesc.Size = deltaObjs * BuffDesc.ElementByteStride;
				BuffDesc.Usage = USAGE_DYNAMIC;
				BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
				BuffDesc.BindFlags = BIND_UNORDERED_ACCESS;// | BIND_SHADER_RESOURCE;;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
				m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_brightObjs);

				BuffDesc.Name = "m_moveProp DF";
				//BuffDesc.Size = deltaChunks * BuffDesc.ElementByteStride;
				BuffDesc.Usage = USAGE_DEFAULT;
				BuffDesc.BindFlags = BIND_UNORDERED_ACCESS;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_NONE;
				m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_deltaObjsL);

				BuffDesc.Name = "m_moveProp";
				//BuffDesc.Size = deltaChunks;
				BuffDesc.Usage = USAGE_DYNAMIC;
				BuffDesc.BindFlags = BIND_SHADER_RESOURCE;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
				m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_deltaObjs);

				BuffDesc.Name = "ConstantsL CB";
				BuffDesc.ElementByteStride = 256;
				BuffDesc.Size = 256;
				BuffDesc.Usage = USAGE_DEFAULT;
				BuffDesc.BindFlags = BIND_UNORDERED_ACCESS | BIND_UNIFORM_BUFFER;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_NONE;
				m_pDevice->CreateBuffer(BuffDesc, nullptr, &ConstantsL);
				//BuffDesc;
				BuffDesc.Name = "Shadowqueue";
				BuffDesc.Size = QueueSize * sizeof(sStruct::CastArg);
				BuffDesc.ElementByteStride = sizeof(sStruct::CastArg);
				BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
				BuffDesc.Usage = USAGE_DEFAULT;
				BuffDesc.BindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_NONE;
				m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_Shadowqueue);

				BuffDesc.Name = "VfArg";
				BuffDesc.ElementByteStride = sizeof(sStruct::VFoutArgs);
				BuffDesc.Size = BuffDesc.ElementByteStride * 4096;
				//BuffDesc.BindFlags = ;
				//BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
				BuffDesc.Usage = USAGE_DEFAULT;
				BuffDesc.BindFlags = BIND_UNORDERED_ACCESS | BIND_INDIRECT_DRAW_ARGS;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_NONE;
				VBData.pData = VfArgspan.data();
				VBData.DataSize = VfArgspan.size_bytes();
				m_pDevice->CreateBuffer(BuffDesc, &VBData, &m_VFArg);


				BuffDesc.Name = "VfArgR";
				BuffDesc.ElementByteStride = sizeof(sStruct::VFoutArgs);;
				BuffDesc.Size = BuffDesc.ElementByteStride * VfArgREle;
				//BuffDesc.BindFlags = ;
				//BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
				BuffDesc.Usage = USAGE_STAGING;
				BuffDesc.BindFlags = BIND_NONE;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_READ;
				m_pDevice->CreateBuffer(BuffDesc, NULL, &m_VFArgR);
				std::vector<StateTransitionDesc> Barriers;
				Barriers.emplace_back(m_VFArgR, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_COPY_DEST, STATE_TRANSITION_FLAG_UPDATE_STATE);

				BuffDesc.Name = "VfOIns";
				BuffDesc.ElementByteStride = sizeof(sStruct::inscmb);
				BuffDesc.Size = Maxinstancnum * BuffDesc.ElementByteStride;
				BuffDesc.Usage = USAGE_DEFAULT;
				BuffDesc.BindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_NONE;
				m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_Vresi);
				Barriers.emplace_back(m_Vresi, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE);
				Barriers.emplace_back(m_Vresi, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_UNORDERED_ACCESS, STATE_TRANSITION_FLAG_UPDATE_STATE);
				m_pImmediateContext->TransitionResourceStates(Barriers.size(), Barriers.data());

				BuffDesc.Name = "Vf_Props";
				BuffDesc.ElementByteStride = sizeof(sStruct::instData1);
				BuffDesc.Size = Maxinstancnum * BuffDesc.ElementByteStride;
				BuffDesc.Usage = USAGE_DYNAMIC;
				BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
				BuffDesc.BindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
				m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_vfProps);

				BuffDesc.Name = "Vf_Trans";
				BuffDesc.ElementByteStride = sizeof(sStruct::mtransfrom);
				BuffDesc.Size = Maxinstancnum * BuffDesc.ElementByteStride;
				BuffDesc.Usage = USAGE_DYNAMIC;
				BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
				BuffDesc.BindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
				m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_vfTrans);

				/*const auto cbv = [&](const int Stride, RefCntAutoPtr<IBuffer>& tbuf) {
					BufferViewDesc ViewDesc;
					for (size_t i1 = 0, elesum=0; i1 < bufinfo.size(); i1++)
					{
						const auto& maxele = bufinfo[i1].count;
						BufferViewDesc ViewDesc;
						ViewDesc.ViewType = BUFFER_VIEW_SHADER_RESOURCE;
						ViewDesc.ByteOffset = elesum * Stride;
						ViewDesc.ByteWidth = maxele * Stride;
						elesum += maxele;
						IBufferView* bv=NULL;
						tbuf->CreateView(ViewDesc, &bv);
						_bfviews.emplace_back(bv);
					}
				};
				cbv(sizeof(sStruct::VfProp), m_vfProps);
				cbv(sizeof(sStruct::mtransfrom), m_vfTrans);*/

				BuffDesc.Name = "so_bufferinfo";
				BuffDesc.ElementByteStride = sizeof(sStruct::bufferinfo);
				BuffDesc.Size = bufnum * BuffDesc.ElementByteStride;
				BuffDesc.Usage = USAGE_DEFAULT;
				BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
				BuffDesc.BindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_NONE;
				m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_soBFinfo);
				
				/*BuffDesc.Name = "co_bufferinfo";
				BuffDesc.ElementByteStride = sizeof(sStruct::bufferinfo);
				BuffDesc.Size = bufnum * BuffDesc.ElementByteStride;
				BuffDesc.Usage = USAGE_DEFAULT;
				BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
				BuffDesc.BindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_NONE;
				m_pDevice->CreateBuffer(BuffDesc, nullptr, &_rdBFinfo);*/

				BuffDesc.Name = "RWSign";
				BuffDesc.Size = (QueueSize / 32) * 4 + 256;
				BuffDesc.Usage = USAGE_DEFAULT;
				BuffDesc.Mode = BUFFER_MODE_FORMATTED;
				BuffDesc.ElementByteStride = 4;
				BuffDesc.BindFlags = BIND_UNORDERED_ACCESS;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_NONE;
				std::vector<UINT32> signs(BuffDesc.Size * 4, 0);
				VBData.pData = signs.data();
				VBData.DataSize = sizeof(UINT32) * static_cast<Uint32>(signs.size());
				m_pDevice->CreateBuffer(BuffDesc, &VBData, &m_RWSign);
				BufferViewDesc ViewDesc;
				ViewDesc.ViewType = BUFFER_VIEW_UNORDERED_ACCESS;
				ViewDesc.Format.ValueType = VT_UINT32;
				ViewDesc.Format.NumComponents = 1;
				m_RWSign->CreateView(ViewDesc, &pRWSignUAV);


				RefCntAutoPtr<ITexture> Tex2d;
				TextureDesc TexDesc;
				TexDesc.Type = RESOURCE_DIM_TEX_2D;
				TexDesc.Usage = Diligent::USAGE::USAGE_DEFAULT;
				//TexDesc.Usage = Diligent::USAGE::USAGE_DYNAMIC;
				TexDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS;
				TexDesc.Width = shadowmapsz;
				TexDesc.Height = shadowmapsz;
				TexDesc.Format = TEX_FORMAT_RGBA32_SINT;
				TexDesc.MipLevels = 1;
				//TextureDesc TexDesc(RESOURCE_DIM_TEX_2D, shadowmapsz, shadowmapsz, 1, TEX_FORMAT_RGBA32_UINT, USAGE_DEFAULT, BIND_SHADER_RESOURCE);
				TexDesc.Name = "shadow texture1";
				m_pDevice->CreateTexture(TexDesc, NULL, &Tex2d);
				m_ShadowMapSRV[0] = Tex2d->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
				m_ShadowMapUAV[0] = Tex2d->GetDefaultView(TEXTURE_VIEW_UNORDERED_ACCESS);
				TexDesc.Name = "shadow texture2";
				Tex2d = NULL;
				m_pDevice->CreateTexture(TexDesc, NULL, &Tex2d);
				m_ShadowMapSRV[1] = Tex2d->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
				m_ShadowMapUAV[1] = Tex2d->GetDefaultView(TEXTURE_VIEW_UNORDERED_ACCESS);

				TexDesc.Name = "resistance texture";
				Tex2d = NULL;
				//TexDesc.BindFlags = BIND_SHADER_RESOURCE;
				std::vector<math::vec4<int>>     Data(TexDesc.Width * TexDesc.Height, math::vec4<int>(1, 1, 1, 1));
				TextureSubResData       Level0Data{ Data.data(), TexDesc.Width * 16 };
				TextureData             InitData{ &Level0Data, 1 };
				m_pDevice->CreateTexture(TexDesc, NULL, &Tex2d);
				m_resistanceSRV = Tex2d->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
				m_resistanceUAV = Tex2d->GetDefaultView(TEXTURE_VIEW_UNORDERED_ACCESS);

				TextureDesc RTColorDesc;
				RTColorDesc.Name = "Offscreen render target";
				RTColorDesc.Type = RESOURCE_DIM_TEX_2D;
				RTColorDesc.Width = m_pSwapChain->GetDesc().Width;
				RTColorDesc.Height = m_pSwapChain->GetDesc().Height;
				RTColorDesc.MipLevels = 1;
				RTColorDesc.Format = FowbatRender::RenderTargetFormat;//m_pSwapChain->GetDesc().ColorBufferFormat;

				// The render target can be bound as a shader resource and as a render target
				RTColorDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_RENDER_TARGET;

				RTColorDesc.ClearValue.Format = RTColorDesc.Format;
				RTColorDesc.ClearValue.Color[0] = 0.350f;
				RTColorDesc.ClearValue.Color[1] = 0.350f;
				RTColorDesc.ClearValue.Color[2] = 0.350f;
				RTColorDesc.ClearValue.Color[3] = 1.f;
				m_RendeRT.Release();
				m_pDevice->CreateTexture(RTColorDesc, nullptr, &pRTColor);
				m_pDevice->CreateTexture(RTColorDesc, nullptr, &pRTColorfow);
				/*TextureViewDesc.AccessFlags = UAV_ACCESS_FLAG_WRITE;
				Tex2d->CreateView(TextureViewDesc,&m_resistanceClearUAV);*/
				/*BufferDesc CBDesc;
				CBDesc.Name = "m_moveObjs";
				CBDesc.Size = deltaChunks;
				CBDesc.Usage = USAGE_DYNAMIC;
				CBDesc.BindFlags = BIND_UNIFORM_BUFFER;
				CBDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
				m_pDevice->CreateBuffer(CBDesc, nullptr, &m_Moveprop);*/
			}
			/*RefCntAutoPtr<ITexture> pRTColor;
			m_pDevice->CreateTexture(RTColorDesc, nullptr, &pRTColor);
			m_RendeRT = pRTColor->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET);
			m_ShadowSRV = pRTColor->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);*/

			//DrawFunc::CreateComputePSO("resetArg", "ShadowCast.hlsl", "resetArg", m_pDevice, pShaderSourceFactory, m_pResetArgPSO, Vars);

			m_ClightPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "Constants")->Set(m_brightcb);
			m_pResetPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "Constants")->Set(m_brightcb);

			Vfilterpsb.PSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "Constants")->Set(m_brightcb);
			/*Vfilterpsb.PSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "so_bufferinfo")->Set(m_soBFinfo);
			Vfilterpsb.PSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "co_bufferinfo")->Set(_rdBFinfo);*/
			//m_ClightPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "queueFB")->Set(m_brightcbl);
			//m_ClightPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "RWSign")->Set(m_RWSign);
			m_pMovePSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "Constants")->Set(m_brightcb);
			m_RfowPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Cbuf")->Set(m_CamConstants);

			m_pMoveSRB.Release();
			m_plightSRB.Release();
			m_ClearSRV.Release();
			for (size_t i = 0; i < m_ShadowMapSRV.size(); i++)
			{
				m_RfowPSO->CreateShaderResourceBinding(&m_fowSRB[i], true);
			}
			m_ClightPSO->CreateShaderResourceBinding(&m_plightSRB, true);
			m_pMovePSO->CreateShaderResourceBinding(&m_pMoveSRB, true);
			m_pResetPSO->CreateShaderResourceBinding(&m_pResetSRB, true);
			m_pResetPSO->CreateShaderResourceBinding(&m_pReset1SRB, true);
			//m_pResetArgPSO->CreateShaderResourceBinding(&m_pResetArgSRB, true);
			m_pRTPSO->CreateShaderResourceBinding(&m_pRTSRB, true);
			Vfilterpsb.CreateShaderResourceBinding();


			for (size_t i = 0; i < m_ShadowMapSRV.size(); i++)
			{
				m_fowSRB[i]->GetVariableByName(SHADER_TYPE_PIXEL, "resistanceMapR")->Set(m_resistanceSRV);
				m_fowSRB[i]->GetVariableByName(SHADER_TYPE_PIXEL, "lightMapfow")->Set(m_ShadowMapSRV[i]);
				Vfilterpsb.SRBs[i]->GetVariableByName(SHADER_TYPE_COMPUTE, "lightMapSRV")->Set(m_ShadowMapSRV[i]);
				Vfilterpsb.SRBs[i]->GetVariableByName(SHADER_TYPE_COMPUTE, "s_objProps")->Set(m_vfProps->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
				Vfilterpsb.SRBs[i]->GetVariableByName(SHADER_TYPE_COMPUTE, "s_objtrans")->Set(m_vfTrans->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
				Vfilterpsb.SRBs[i]->GetVariableByName(SHADER_TYPE_COMPUTE, "so_bufferinfo")->Set(m_soBFinfo->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
				Vfilterpsb.SRBs[i]->GetVariableByName(SHADER_TYPE_COMPUTE, "VfOIns")->Set(m_Vresi->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
				Vfilterpsb.SRBs[i]->GetVariableByName(SHADER_TYPE_COMPUTE, "VfArg")->Set(m_VFArg->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
			}

			m_pMoveSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "resistanceMapW")->Set(m_resistanceUAV);
			m_pMoveSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "moveProp")->Set(m_deltaObjs->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
			m_pMoveSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "movePropLocal")->Set(m_deltaObjsL->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
			m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "resistanceMap")->Set(m_resistanceSRV);;
			//m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "queueFB")->Set(queueFB->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
			m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWSign")->Set(pRWSignUAV);
			m_lightMapUAV = m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "lightMap");
			m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "Shadowqueue")->Set(m_Shadowqueue->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
			m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "objProp")->Set(m_brightObjs->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
			//m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWSign")->Set(pRWSignUAV);
			//m_pImmediateContext->buf
			//m_LSRV= m_pMoveSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "lightMap");
			m_ClearSRV = m_pResetSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "clearlightMap");
			m_pResetSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "VfArg")->Set(m_VFArg->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
			m_pReset1SRB->GetVariableByName(SHADER_TYPE_COMPUTE, "VfArg")->Set(m_VFArg->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
			auto GroupMap = m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "GroupMap");
			//m_TargetSRV = m_fowSRB->GetVariableByName(SHADER_TYPE_PIXEL, "fowresult");
			m_pRTSRB->GetVariableByName(SHADER_TYPE_PIXEL, "fowresult")->Set(tview);;
			m_TargetMapSRV = m_pRTSRB->GetVariableByName(SHADER_TYPE_PIXEL, "lightMapfow");
			m_TargetMapSRV->Set(pRTColorfow->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
			//m_RendeRT= pRTColor->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET);
			m_RendeRTf = pRTColorfow->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET);
			//m_pRTSRB->GetVariableByName(SHADER_TYPE_PIXEL, "fowresult")->Set(pRWSignUAV);

			m_pReset1SRB->GetVariableByName(SHADER_TYPE_COMPUTE, "clearlightMap")->Set(m_resistanceUAV);
			/*m_lightMapUAV->Set(m_ShadowMapUAV[0]);
			m_lightMapUAV1->Set(m_ShadowMapUAV[1]);*/

			auto* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
			gCons.mapsize = { shadowmapsz ,shadowmapsz };
			gCons.lightcolor = { 2 ,2,2,2 };
		}

		template <typename C>
		constexpr void Updateinsdata(C&& fc) {
			//MapHelper<T> m_MappedData(m_pImmediateContext, m_Vresi, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
			fc(m_vfProps);
		}

		sStruct::GlobalConstants gCons;
		math::ui64 ctex = 0, ntex = 0;
		template<typename C>
		void Cull(RefCntAutoPtr<IDeviceContext>& m_pImmediateContext,
			nonstd::span<sStruct::objrep> Lighteobjs, //std::vector<sStruct::VFoutArgs> drawresult,
			C&& PrepareFunc,
			int insTancenum,
			int MaxArgnum = 100) {
			gCons.inum = MaxArgnum;
			gCons.pedd = MaxArgnum;
			ctex = Tim % m_ShadowMapSRV.size(), ntex = (Tim += 1) % m_ShadowMapSRV.size();
			auto& cSRV = m_ShadowMapSRV[ctex];
			auto& nextUAV = m_ShadowMapUAV[ntex];
			auto& cUAV = m_ShadowMapUAV[ctex];
			//m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "objProp")->Set(m_brightObjs->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));

			//m_TargetMapSRV->Set(cSRV);
			//clear next lightmap
			/*m_ClearSRV->Set(nextUAV);
			m_pImmediateContext->SetPipelineState(m_pResetArgPSO);
			m_pImmediateContext->CommitShaderResources(m_pResetArgSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
			DispatAttribs.ThreadGroupCountX = gCons.pedd/m_ThreadGroupSize+1;
			m_pImmediateContext->DispatchCompute(DispatAttribs);*/

			m_ClearSRV->Set(m_ShadowMapUAV[ntex]);
			m_pImmediateContext->SetPipelineState(m_pResetPSO);
			{
				MapHelper<sStruct::objrep> m_MappedData(m_pImmediateContext, m_brightObjs, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
				std::memcpy(m_MappedData, Lighteobjs.data(), Lighteobjs.size_bytes());
				for (size_t i = 0; i < Lighteobjs.size(); i++)
				{
					m_MappedData[i].pos.head<2>() += (math::int2::Constant(shadowmapsz / 2));
				}
				Diligent::MapHelper<sStruct::GlobalConstants> gconst(m_pImmediateContext, m_brightcb, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
				*gconst = gCons;
				gconst->uiNum = Lighteobjs.size();
			}
			m_pImmediateContext->CommitShaderResources(m_pResetSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			//clear 4x4 block each thread
			DispatAttribs.ThreadGroupCountX = (shadowmapsz / 32);
			DispatAttribs.ThreadGroupCountY = shadowmapsz / 32;
			m_pImmediateContext->DispatchCompute(DispatAttribs);


			m_lightMapUAV->Set(cUAV);
			m_pImmediateContext->SetPipelineState(m_ClightPSO);
			m_pImmediateContext->CommitShaderResources(m_plightSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			int gcx = (Lighteobjs.size() * 8) / m_ThreadGroupSize;
			DispatAttribs.ThreadGroupCountX = gcx + 1;
			DispatAttribs.ThreadGroupCountY = 1;
			DispatAttribs.ThreadGroupCountZ = 1;
			m_pImmediateContext->DispatchCompute(DispatAttribs);
			if (insTancenum != 0) [[likely]]
				//cull
			{
				auto & pCtx = m_pImmediateContext;
				m_pImmediateContext->SetPipelineState(Vfilterpsb.PSO);
				m_pImmediateContext->CommitShaderResources(Vfilterpsb.SRBs[ctex], RESOURCE_STATE_TRANSITION_MODE_VERIFY);
				gcx = (insTancenum * 8) / m_ThreadGroupSize;
				DispatAttribs.ThreadGroupCountX = gcx + 1;
				DispatAttribs.ThreadGroupCountY = 1;
				DispatAttribs.ThreadGroupCountZ = 1;
				m_pImmediateContext->DispatchCompute(DispatAttribs);

				pCtx->CopyBuffer(m_VFArg, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, m_VFArgR, 0, sizeof(sStruct::VFoutArgs) * MaxArgnum, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
				void* pMappedData = nullptr;
				pCtx->WaitForIdle();
				pCtx->MapBuffer(m_VFArgR, MAP_READ, MAP_FLAG_DO_NOT_WAIT, pMappedData);
				PrepareFunc(nonstd::span(reinterpret_cast<sStruct::VFoutArgs*>(pMappedData),MaxArgnum),m_VFArg);
				pCtx->UnmapBuffer(m_VFArgR, MAP_READ);
			}
		}
		//clean lightMap feed lightMap -> calculate lightMap -> pixel sample-> blend with input tex
		void Render(const Camera cam, RefCntAutoPtr<IDeviceContext>& m_pImmediateContext,
			nonstd::span<sStruct::objrep> Lighteobjs) {
			//const math::ui64 ctex = Tim % m_ShadowMapSRV.size(), ntex = (Tim) % m_ShadowMapSRV.size();
				{
					auto& pCtx = m_pImmediateContext;
					{
						auto& pRTV = m_RendeRTf;//  m_pSwapChain->GetCurrentBackBufferRTV();

						m_pImmediateContext->SetRenderTargets(1, &pRTV, m_pSwapChain->GetDepthBufferDSV(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
						m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
						m_pImmediateContext->ClearDepthStencil(m_pSwapChain->GetDepthBufferDSV(), CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
					}
					IBuffer* pVBs[] = { plane.pVertexBuffer };
					math::ui64   Offsets[_countof(pVBs)] = {};
					{
						struct CameraAttribs
						{
							using float4 = math::Vector4f;
							using float2 = math::Vector2f;
							using float4x4 = math::Matrix4f;
							float4 f4Position;     // Camera world position
							float4 f4ViewportSize; // (width, height, 1/width, 1/height)

							float2 f2ViewportOrigin; // (min x, min y)
							float fNearPlaneZ;
							float fFarPlaneZ; // fNearPlaneZ < fFarPlaneZ
							float4x4 mViewT;
							float4x4 mProjT;
							float4x4 mViewProjT;
							float4x4 mViewInvT;
							float4x4 mProjInvT;
							float4x4 mViewProjInvT;
							float4 f4ExtraData[5]; // Any appliation-specific data
							// Sizeof(CameraAttribs) == 256*2
						};
						//m_lightMapfowSRV->Set(rendertex);
						MapHelper<CameraAttribs> CamAttribs(m_pImmediateContext, m_CamConstants, MAP_WRITE, MAP_FLAG_DISCARD);
						(CamAttribs->mProjT) = (cam.GetProjMatrix().transpose());
						CamAttribs->mViewProjT = cam.CameraViewProj;
						CamAttribs->mViewProjInvT = cam.CameraViewProj.inverse();
						CamAttribs->f4Position = cam.GetWorldMatrix().col(3);// Diligent::float4(Diligent::float3::MakeVector(cam.GetWorldMatrix()[3]), 1);
						CamAttribs->f4Position[3] = 1;
						//grid size
						CamAttribs->f4ExtraData[0] = { 4,4,0,1 };
						//offset to positive
						CamAttribs->f4ExtraData[1] = { shadowmapsz / 2.0f,shadowmapsz / 2.0f,0.0f,1.0f };
						//= ex;
					}
					//pCtx->TransitionResourceStates(_countof(Barriers), Barriers);
					pCtx->SetVertexBuffers(0, _countof(pVBs), pVBs, Offsets, RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_NONE);
					pCtx->SetIndexBuffer(plane.m_IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
					pCtx->SetPipelineState(m_RfowPSO);
					m_pImmediateContext->CommitShaderResources(m_fowSRB[ctex], RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
					DrawIndexedAttribs drawAttrs(plane.IndexCount, VT_UINT32, DRAW_FLAG_VERIFY_ALL);
					pCtx->DrawIndexed(drawAttrs);
				}
				{
					auto pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
					m_pImmediateContext->SetRenderTargets(1, &pRTV, m_pSwapChain->GetDepthBufferDSV(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
					m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
					m_pImmediateContext->ClearDepthStencil(m_pSwapChain->GetDepthBufferDSV(), CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
					// //Set the render target pipeline state
					m_pImmediateContext->SetPipelineState(m_pRTPSO);
					//// Commit the render target shader's resources
					m_pImmediateContext->CommitShaderResources(m_pRTSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
					// Draw the render target's vertices
					DrawAttribs RTDrawAttrs;
					RTDrawAttrs.NumVertices = 4;
					RTDrawAttrs.Flags = DRAW_FLAG_VERIFY_ALL; // Verify the state of vertex and index buffers
					m_pImmediateContext->Draw(RTDrawAttrs);
				}
		}

		void AddOneBlock(sStruct::objrep o) {
			o.pos.head<2>().array() += shadowmapsz / 2;
			/*void* p_deltaObjs = NULL;
			auto flag = p_deltaObjs == NULL ? MAP_FLAGS::MAP_FLAG_DISCARD : MAP_FLAGS::MAP_FLAG_NO_OVERWRITE;
			m_pImmediateContext->MapBuffer(m_deltaObjs, MAP_TYPE::MAP_WRITE, flag, p_deltaObjs);*/
			Diligent::MapHelper<sStruct::GlobalConstants> gconst(m_pImmediateContext, m_brightcb, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
			gconst->uiNum = 1;
			gconst->mapsize = { shadowmapsz ,shadowmapsz };
			MapHelper<Uint8> m_MappedData;
			m_MappedData.Map(m_pImmediateContext, m_deltaObjs, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
			std::memcpy(m_MappedData, &o, sizeof(sStruct::objrep));
			m_pImmediateContext->SetPipelineState(m_pMovePSO);
			m_pImmediateContext->CommitShaderResources(m_pMoveSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			DispatAttribs.ThreadGroupCountX = 1;
			m_pImmediateContext->DispatchCompute(DispatAttribs);
		}
		void AddBlock(nonstd::span<sStruct::objrep> o) {
			/*void* p_deltaObjs = NULL;
			auto flag = p_deltaObjs == NULL ? MAP_FLAGS::MAP_FLAG_DISCARD : MAP_FLAGS::MAP_FLAG_NO_OVERWRITE;
			m_pImmediateContext->MapBuffer(m_deltaObjs, MAP_TYPE::MAP_WRITE, flag, p_deltaObjs);*/
			{
				Diligent::MapHelper<sStruct::GlobalConstants> gconst(m_pImmediateContext, m_brightcb, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
				gconst->uiNum = o.size();
				gconst->mapsize = { shadowmapsz ,shadowmapsz };
				MapHelper<Uint8> m_MappedData;
				m_MappedData.Map(m_pImmediateContext, m_deltaObjs, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
				std::memcpy(m_MappedData, o.data(), o.size_bytes());
			}
			m_pImmediateContext->SetPipelineState(m_pMovePSO);
			m_pImmediateContext->CommitShaderResources(m_pMoveSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			DispatAttribs.ThreadGroupCountX = (o.size() + m_ThreadGroupSize - 1) / m_ThreadGroupSize;
			m_pImmediateContext->DispatchCompute(DispatAttribs);
		}
		void GenerateBlock(Utility::span<sStruct::objrep> bks, math::int4 size = { 100,100,0,0 }) {
			//(num);
			auto offset = size.head<2>() / -2 + size.tail(2);// +math::int2::Constant(shadowmapsz / 2);
			for (size_t i = 0; i < bks.size(); i++)
			{

				auto& bk = bks[i];
				bk.pos[0] = std::rand() % size[0];
				bk.pos[1] = std::rand() % size[1];
				bk.pos[2] = 3;
				bk.pos.head<2>() += offset;
			}
			AddBlock(bks);
		}

		~FowbatRender() {}
		static constexpr float ClearColor[] = { 0.35, 0.35, 0.35, 1 };
		int shadowmapsz = 1024, Texsize = 512;
		int64_t Tim = 0;
		RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
		std::array<RefCntAutoPtr<ITextureView>, 2>           m_ShadowMapSRV;
		std::array<RefCntAutoPtr<ITextureView>, 2>           m_ShadowMapUAV;
		/*void* p_deltaObjs = NULL;
		void* p_brightObjs = NULL;*/
		RefCntAutoPtr<ITextureView>           m_resistanceSRV;
		RefCntAutoPtr<ITextureView>           m_resistanceUAV;
		RefCntAutoPtr<ITextureView>           m_resistanceClearUAV;
		RefCntAutoPtr<ITextureView>           m_RendeRT;
		RefCntAutoPtr<ITextureView>           m_RendeRTf;
		RefCntAutoPtr<ITextureView>           m_ShadowSRV;

		static constexpr TEXTURE_FORMAT RenderTargetFormat = TEX_FORMAT_RGBA8_UNORM;
		static constexpr TEXTURE_FORMAT DepthBufferFormat = TEX_FORMAT_D32_FLOAT;
		RefCntAutoPtr<IShaderResourceVariable>  m_lightMapUAV;
		RefCntAutoPtr<IShaderResourceVariable>  m_lightMapUAV1;
		RefCntAutoPtr<IShaderResourceVariable>  m_ShadowMap1SRV;
		RefCntAutoPtr<IShaderResourceVariable>  m_ClearSRV;
		RefCntAutoPtr<IShaderResourceVariable>  m_TargetMapSRV;
		RefCntAutoPtr<IShaderResourceVariable>  m_lightMapfowSRV;
		RefCntAutoPtr<IPipelineState>         m_pResetPSO;
		RefCntAutoPtr<IShaderResourceBinding> m_pResetSRB;
		RefCntAutoPtr<IPipelineState>         m_pResetArgPSO;
		RefCntAutoPtr<IShaderResourceBinding> m_pResetArgSRB;
		RefCntAutoPtr<IShaderResourceBinding> m_pReset1SRB;
		RefCntAutoPtr<IShaderResourceBinding> m_pReset2SRB;
		RefCntAutoPtr<IPipelineState>         m_pMovePSO;
		RefCntAutoPtr<IShaderResourceBinding> m_pMoveSRB;
		RefCntAutoPtr<IPipelineState> m_ClightPSO;
		RefCntAutoPtr<IShaderResourceBinding> m_plightSRB;
		RefCntAutoPtr<IPipelineState> m_RfowPSO;
		RefCntAutoPtr<IShaderResourceBinding> m_fowSRB[2];
		RefCntAutoPtr<IShaderResourceBinding> m_fowSRB0;
		RefCntAutoPtr<IPipelineState> m_pRTPSO;
		RefCntAutoPtr<IShaderResourceBinding> m_pRTSRB;
		RefCntAutoPtr<ISwapChain> m_pSwapChain;
		RenderUtil::PSB<2> Vfilterpsb;
	private:
		DispatchComputeAttribs DispatAttribs;

	};
}