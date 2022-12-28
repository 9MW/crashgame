
//#define MINIAUDIO_IMPLEMENTATION
#define STB_VORBIS_HEADER_ONLY
#include "miniaudio/extras/stb_vorbis.c"  // Enables Vorbis decoding.
//#define MINIAUDIO_IMPLEMENTATION
#include"miniaudio/miniaudio.h"
#include"GProps.h"
#include"Render/RenderUtil.h"
#include"Macros.h"
#include"Render/ShaderStructs.h"
#include "MapHelper.hpp"
#include <filesystem>
//#include <Render/RenderUtil.cpp>
namespace fs = std::filesystem;
export module audio0;
EXPO
//export void MyFunc();
namespace pj2::audio {
#define SAMPLE_FORMAT   ma_format_f32
#define CHANNEL_COUNT   2
#define SAMPLE_RATE     44100
#define DEVICE_FORMAT           ma_format_f32
#define DEVICE_CHANNELS         2
#define DEVICE_SAMPLE_RATE      44100
	using IBuffer = Diligent::IBuffer;
	using v4f = math::Vector4f;
	using Char2 = wchar_t;
	constexpr int abufnm = 4;
	constexpr int maxiumLatency = 2;
	constexpr int  audiobufsize = 4096;
	//EXPO
	EXPO
	//template<int audiobufsize = 4096>
	struct AudioManager
	{
	private:

		ma_event g_stopEvent; /* <-- Signaled by the audio thread, waited on by the main thread. */
		ReUtS::t_psb1 _pb1;
		ma_device device;
	public:
		Diligent::RefCntAutoPtr<IBuffer>                VfArg;
		Diligent::RefCntAutoPtr<IBuffer>                audio_bufin;
		Diligent::RefCntAutoPtr<IBuffer>                audio_bufout;
		Diligent::RefCntAutoPtr<IBuffer>                v_audio_bufout;
		Diligent::RefCntAutoPtr<IBuffer>                audio_inst;
		RenderContext _rc;
		std::array<float[audiobufsize], abufnm> audiobuf;
		std::vector<math::v4f> daudios;
		ui64 fram = 1,prevaudiofram=1;
		Diligent::RefCntAutoPtr<Diligent::IFence> _copyfen;
		ma_decoder_config decoderConfig;
		//preallocate 20MB buffer
		void LoadAudio(const std::string path,ui64 totalsize=1024*1024*20) {
			daudios.resize(totalsize / 16);
			auto&& buf0 = daudios;
			ma_decoder g_pDecoders[1];
			decoderConfig = ma_decoder_config_init(SAMPLE_FORMAT, CHANNEL_COUNT, SAMPLE_RATE);
			ma_result result;
			constexpr int iDecoder = 0;
			for (const auto& entry : fs::directory_iterator(path)) {
				result = ma_decoder_init_file_w(entry.path().c_str(), &decoderConfig, &g_pDecoders[iDecoder]);
				//result=
				if (result == MA_SUCCESS) {
					int frmpross = 0, bkpros = 4096;
					while (true)
					{
						ma_uint32 framesRead = ma_decoder_read_pcm_frames(&g_pDecoders[iDecoder], buf0.data() + frmpross, bkpros);
						frmpross += framesRead;
						if (framesRead < bkpros)
							break;
						if (frmpross + bkpros >= buf0.size())
							buf0.resize(buf0.size() * 2);
					}
				}
				else
				{
					LOGINFO("fail load audio file {0}", entry.path());
				}
				ma_decoder_uninit(&g_pDecoders[iDecoder]);
			}
		}
		void init(const RenderContext& rt) {
			{

				ma_device_config deviceConfig;
				deviceConfig = ma_device_config_init(ma_device_type_playback);
				deviceConfig.playback.format = DEVICE_FORMAT;
				deviceConfig.playback.channels = DEVICE_CHANNELS;
				deviceConfig.sampleRate = DEVICE_SAMPLE_RATE;
				deviceConfig.dataCallback = data_callback;
				deviceConfig.pUserData = this;  /* <-- Set this to a pointer to the ring buffer if you don't want it in global scope. */
				deviceConfig.periodSizeInMilliseconds = logicstep;
				if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
					assert(false, "Failed to open playback device.\n");
				}
				if (ma_device_start(&device) != MA_SUCCESS) {
					ma_device_uninit(&device);
					/*for (iDecoder = 0; iDecoder < g_decoderCount; ++iDecoder) {
						ma_decoder_uninit(&g_pDecoders[iDecoder]);
					}
					free(g_pDecoders);
					free(g_pDecodersAtEnd);*/

					assert(false,"Failed to start playback device.\n");
				}
				ma_event_init(&g_stopEvent);
			}
			_rc = rt;
			auto& rt_ = _rc;
			auto&& m_pDevice = rt_.m_pDevice;

			using namespace Diligent;
			FenceDesc fenceDesc;
			fenceDesc.Name = "audio buffer copy fence";
			m_pDevice->CreateFence(fenceDesc, &_copyfen);
			Diligent::ShaderResourceVariableDesc Vars[] =
			{
				//{SHADER_TYPE_COMPUTE, "queueFB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
				{Diligent::SHADER_TYPE_COMPUTE, "Constants",   Diligent::SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
				{Diligent::SHADER_TYPE_COMPUTE, "VfArg",       Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
				{Diligent::SHADER_TYPE_COMPUTE, "audio_bufin", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
				{Diligent::SHADER_TYPE_COMPUTE, "audio_bufout",Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
				{Diligent::SHADER_TYPE_COMPUTE, "audio_inst",  Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
			};
			pj2::DrawFunc::CreateComputePSO("mixer0", "audio.hlsl", "mixer0", m_pDevice,
				rt_.pShaderSourceFactory, _pb1.PSO, Vars, 256);

			_pb1.CreateShaderResourceBinding();
			using BufferData = Diligent::BufferData;
			using BufferDesc = Diligent::BufferDesc;
			BufferData VBData;
			BufferDesc BuffDesc;
			auto&& _SRB = _pb1.GetShaderResourceBinding(0);
			auto CreateSetBUf = [&](RefCntAutoPtr<IRenderDevice>& m_pDevice,const BufferDesc BuffDesc, IBuffer** ppBuffer, RefCntAutoPtr<IShaderResourceBinding>& psrb
				//, Diligent::SHADER_TYPE ST, const BufferData* pBuffData
				) {
				RenderUtil::CreateSetBUf(m_pDevice, rt_.m_pImmediateContext, BuffDesc, ppBuffer, psrb);
			};
			//using namespace RenderUtil;
			{
				using ts = math::v4f;
				BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
				/*VBData.DataSize = fdf.size() * 16;
				VBData.pData = fdf.data();*/
				RenderUtil::md_BufferDesc("audio_bufin", BuffDesc, audiobufsize, sizeof(ts), USAGE_DEFAULT, BIND_SHADER_RESOURCE, CPU_ACCESS_NONE);
				CreateSetBUf(m_pDevice, BuffDesc, &audio_bufin, _SRB);
				//m_pDevice->CreateBuffer(BuffDesc,nullptr, &audio_bufin);
			}
			{
				using ts = math::v4f;
				RenderUtil::md_BufferDesc("v_audio_bufout", BuffDesc, audiobufsize, sizeof(ts), USAGE_STAGING, BIND_NONE, CPU_ACCESS_READ);
				CreateSetBUf(m_pDevice, BuffDesc, &v_audio_bufout, _SRB);
				//m_pDevice->CreateBuffer(BuffDesc, nullptr, &v_audio_bufout);
			}
			{
				using ts = math::v4f;
				RenderUtil::md_BufferDesc("audio_bufout", BuffDesc, audiobufsize* audiobuf.size(), sizeof(ts), USAGE_DEFAULT, BIND_UNORDERED_ACCESS, CPU_ACCESS_NONE);
				CreateSetBUf(m_pDevice, BuffDesc, &audio_bufout, _SRB);
				//m_pDevice->CreateBuffer(BuffDesc, nullptr, &audio_bufout);
			}
			{
				using ts = sStruct::VFoutArgs;
				RenderUtil::md_BufferDesc("VfArg", BuffDesc, 1, sizeof(ts), USAGE_DEFAULT, BIND_SHADER_RESOURCE, CPU_ACCESS_NONE);
				CreateSetBUf(m_pDevice, BuffDesc, &VfArg, _SRB);
				//m_pDevice->CreateBuffer(BuffDesc, nullptr, &VfArg);
			}
			{
				using ts = sStruct::Audio0;
				RenderUtil::md_BufferDesc("audio_inst", BuffDesc, 1024, sizeof(ts), USAGE_DYNAMIC, BIND_SHADER_RESOURCE, CPU_ACCESS_WRITE);
				CreateSetBUf(m_pDevice, BuffDesc, &audio_inst, _SRB);
				//m_pDevice->CreateBuffer(BuffDesc, nullptr, &audio_inst);
			}

			/*auto pC = _pb1.PSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "Constants");
			if (pC) {
				pC->Set(m_brightcb);
			}*/
		}
		inline auto getbufpos(const ui64& FrameId) {
			return (FrameId % maxiumLatency)* audiobufsize;
		}
		void Update(int audio_ins) {

			using namespace Diligent;
			auto&& m_pImmediateContext = _rc.m_pImmediateContext;
			DispatchComputeAttribs DispatAttribs(audio_ins, 1, 1);
			m_pImmediateContext->SetPipelineState(_pb1.PSO);
			m_pImmediateContext->CommitShaderResources(_pb1.GetShaderResourceBinding(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			m_pImmediateContext->DispatchCompute(DispatAttribs);
			const auto dist = getbufpos(fram ) ;
			m_pImmediateContext->CopyBuffer(audio_bufout, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, v_audio_bufout,
				dist, audiobufsize, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			m_pImmediateContext->EnqueueSignal(_copyfen, fram);
			Uint64 PrevFrameId = _copyfen->GetCompletedValue();
			if(fram-PrevFrameId> maxiumLatency){
				PrevFrameId = fram - maxiumLatency;
				m_pImmediateContext->WaitForFence(_copyfen, PrevFrameId,false);
			}
			{
				using ts = v4f;
				Diligent::MapHelper<ts> m_MappedData(m_pImmediateContext, v_audio_bufout, MAP_TYPE::MAP_READ, MAP_FLAGS::MAP_FLAG_DO_NOT_WAIT);
				std::memcpy(audiobuf[0], &m_MappedData[getbufpos(PrevFrameId)], audiobufsize);
				/*for (size_t i = 0; i < audio_ins; i++)
				{
					assert(m_MappedData[i] == (math::v4f(0, 1, 2, 3) + math::v4f::Ones() * 4 * i) * 500);
				}
				INFO("sum res={0}", m_MappedData[0].transpose());*/
			}
			fram++;
		}

		/*using t_clock = std::chrono::high_resolution_clock;
		t_clock clockprev;*/
		ui64 audiol1= 0,audiol0=0;
		inline void Play_cb(ma_device* pDevice ,void*& pOutput, ma_uint32 frameCount){
			const auto PrevFrameId = fram - maxiumLatency;
			audiol0 = audiol1 == PrevFrameId ? audiol0 : 0;
			if (audiol0 >= audiobufsize) {
				return;
			}
			audiol1 = PrevFrameId;
			prevaudiofram = PrevFrameId;
			auto& audioframe = audiobuf[PrevFrameId];
			/*frameCount -= frameCount % 4;
			frameCount /= 4;*/
			size_t totalFramesRead = 0;
			float* pOutputF32 = (float*)pOutput;
			//std::memcpy(pOutput,audiobuf[PrevFrameId],)
			for (size_t i = 0; i < frameCount; i++)
			{
				//auto& d=
				for (size_t i1 = 0; i1 < CHANNEL_COUNT; i1++)
				{
					pOutputF32[i * CHANNEL_COUNT + i1] += audioframe[i];
				}
			}
			audiol0 += frameCount;
			//audioframe++;
		}
		static inline void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
		{
			reinterpret_cast<AudioManager*>(pDevice->pUserData)->Play_cb(pDevice,pOutput,frameCount);

			/* Unused in this example. */
			(void)pDevice;
			(void)pInput;
		}
	};
}