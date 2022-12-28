#pragma once
#include "AudioManager.h"
#undef var
#include"audio_vorbis.h"
/*#define STB_VORBIS_HEADER_ONLY
#include "miniaudio/extras/stb_vorbis.c"*/  // Enables Vorbis decoding.
#define MINIAUDIO_IMPLEMENTATION
#include"miniaudio/miniaudio.h"
namespace pj2::Audio {
	constexpr int _szCpy = 4096 * 4;
	inline void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
	{
		reinterpret_cast<AudioManager*>(pDevice->pUserData)->Play_cb(pOutput, frameCount);

		/* Unused in this example. */
		(void)pDevice;
		(void)pInput;
	}
	//preallocate
	ma_event g_stopEvent; /* <-- Signaled by the audio thread, waited on by the main thread. */
	ma_device device;
	ma_decoder_config decoderConfig;

	//preallocate 20MB buffer

	std::array<Diligent::StateTransitionDesc, 2> Barriers0;
	 void AudioManager::LoadAudios(const std::string path, const ui64 totalsize, const ui64 Channel, const ui64 samprate) {
		 bool resize = (daudios.size() < totalsize / 16);
		 if(resize)
		daudios.resize(totalsize / 16);
		
		_audiomap.clear();
		auto&& buf0 = daudios;
		ma_decoder g_pDecoders[1];
		decoderConfig = ma_decoder_config_init(SAMPLE_FORMAT, Channel, samprate);
		ma_result result;
		constexpr int iDecoder = 0;
		audioinfo ainfo;
		ainfo.Channel = Channel;
		ainfo.samprate = samplerate;
		int cpunum = std::thread::hardware_concurrency();
		cpunum = cpunum > 0 ? cpunum : 4;
		ui64 frmpross = 0, bkpros = 4096;
		constexpr int uv = 4;
		for (const auto& entry : fs::directory_iterator(path)) {
			if (!entry.is_regular_file())
				continue;
			result = ma_decoder_init_file_w(entry.path().c_str(), &decoderConfig, &g_pDecoders[iDecoder]);
			//result=
			if (result == MA_SUCCESS) {
				//frmpross += frmpross % 4;
				ainfo.offset = frmpross;
				while (true)
				{
					const auto minsize = frmpross + bkpros/ uv;
					if (minsize>= buf0.size())
						buf0.resize(std::max(buf0.size() * 2, frmpross + bkpros));
					ma_uint32 framesRead = ma_decoder_read_pcm_frames(&g_pDecoders[iDecoder], buf0.data() + frmpross, bkpros);
					frmpross += (framesRead/ uv);
					if (framesRead < bkpros)
						break;
				}
				ainfo.size = frmpross- ainfo.offset;
				auto pair=_audiomap.try_emplace(entry.path().filename().string(), ainfo);
			}
			else
			{
				LOGINFO("fail to load audio file {0}", entry.path());
			}
			ma_decoder_uninit(&g_pDecoders[iDecoder]);
		}
		return;
		auto& rt_ = _rc;
		auto&& _SRB = _pb1.GetShaderResourceBinding(0);
		using BufferData = Diligent::BufferData;
		using BufferDesc = Diligent::BufferDesc;
		using namespace Diligent;
		BufferData VBData;
		BufferDesc BuffDesc;
		using ts = math::v4f;
		BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
		if(resize){
			RenderUtil::md_BufferDesc("audio_bufin", BuffDesc, buf0.size(), sizeof(ts), USAGE_DEFAULT, BIND_SHADER_RESOURCE, CPU_ACCESS_NONE);
			audio_bufin = NULL;
			RenderUtil::CreateSetBUf(rt_.m_pDevice, rt_.m_pImmediateContext, BuffDesc, &audio_bufin, _SRB);
		}

		rt_.m_pImmediateContext->UpdateBuffer(audio_bufin, 0, frmpross * sizeof(ts), buf0.data(),
			Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	}

	 void AudioManager::init(const RenderContext& rt, const ui64 maxaudiosource) {
		 {

			 ma_device_config deviceConfig;
			 deviceConfig = ma_device_config_init(ma_device_type_playback);
			 deviceConfig.playback.format = DEVICE_FORMAT;
			 deviceConfig.playback.channels = DEVICE_CHANNELS;
			 deviceConfig.sampleRate = samplerate;
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

				 assert(false, "Failed to start playback device.\n");
			 }
			 ma_event_init(&g_stopEvent);
		 }
		 return;
		 _rc = rt;
		 auto& rt_ = _rc;
		 auto&& m_pDevice = rt_.m_pDevice;

		 using namespace Diligent;
		 FenceDesc fenceDesc;
		 fenceDesc.Name = "audio buffer copy fence";
		 _copyfen = NULL;
		 m_pDevice->CreateFence(fenceDesc, &_copyfen);
		 Diligent::ShaderResourceVariableDesc Vars[] =
		 {
			 { Diligent::SHADER_TYPE_COMPUTE, "Constants",   Diligent::SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
			 { Diligent::SHADER_TYPE_COMPUTE, "VfArg",       Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			 { Diligent::SHADER_TYPE_COMPUTE, "audio_bufin", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
			 { Diligent::SHADER_TYPE_COMPUTE, "audio_bufout",Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			 { Diligent::SHADER_TYPE_COMPUTE, "audio_inst",  Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			 { Diligent::SHADER_TYPE_COMPUTE, "RWVfArg",     Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			 { Diligent::SHADER_TYPE_COMPUTE, "Vf_Props",    Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			 { Diligent::SHADER_TYPE_COMPUTE, "audio_inst_out",Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			 { Diligent::SHADER_TYPE_COMPUTE, "cbCameraAttribs", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_STATIC  }
		 };
		 _pb1.Release();
		 _pbfilter.Release();
		 pj2::DrawFunc::CreateComputePSO("mixer0", "audio.hlsl", "mixer0", m_pDevice,
			 rt_.pShaderSourceFactory, _pb1.PSO, Vars, 256);
		 pj2::DrawFunc::CreateComputePSO("Vfilter", "audio.hlsl", "Vfilter", m_pDevice,
			 rt_.pShaderSourceFactory, _pbfilter.PSO, Vars);
		 _pbfilter.PSO->GetStaticVariableByName(Diligent::SHADER_TYPE_COMPUTE, "cbCameraAttribs")->Set(cb_camera);
		 _pb1.CreateShaderResourceBinding();
		 //_pbfilter.CreateShaderResourceBinding();
		 using BufferData = Diligent::BufferData;
		 using BufferDesc = Diligent::BufferDesc;
		 BufferData VBData;
		 BufferDesc BuffDesc;
		 auto&& _SRB = _pb1.GetShaderResourceBinding(0);
		 auto&& _SRBfilter = _pbfilter.GetShaderResourceBinding(0);
		 auto CreateSetBUf = [&](RefCntAutoPtr<IRenderDevice>& m_pDevice, const BufferDesc BuffDesc, IBuffer** ppBuffer, RefCntAutoPtr<IShaderResourceBinding>& psrb
			 ) {
				 RenderUtil::CreateSetBUf(m_pDevice, rt_.m_pImmediateContext, BuffDesc, ppBuffer, psrb);
		 };
		 BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
		 {
			 using ts = math::v4f;
			 RenderUtil::md_BufferDesc("v_audio_bufout", BuffDesc, audiobufsize * audiobuf.size(), sizeof(ts), USAGE_STAGING, BIND_NONE, CPU_ACCESS_READ);
			 CreateSetBUf(m_pDevice, BuffDesc, &v_audio_bufout, _SRB);
		 }
		 {
			 using ts = math::v4f;
			 RenderUtil::md_BufferDesc("audio_bufout", BuffDesc, audiobufsize * audiobuf.size(), sizeof(ts), USAGE_DEFAULT, BIND_UNORDERED_ACCESS, CPU_ACCESS_NONE);
			 CreateSetBUf(m_pDevice, BuffDesc, &audio_bufout, _SRB);
		 }
		 /*{
			 using ts = sStruct::VFoutArgs;
			 RenderUtil::md_BufferDesc("VfArg", BuffDesc, 1, sizeof(ts), USAGE_DEFAULT, BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE, CPU_ACCESS_NONE);
			 if(!VfArg)
			 RenderUtil::CreateSetBUf(m_pDevice, rt_.m_pImmediateContext, BuffDesc, &VfArg,
				 _SRB, Diligent::SHADER_TYPE_COMPUTE, nullptr,
				 BIND_SHADER_RESOURCE);
			 _SRB->GetVariableByName(SHADER_TYPE_COMPUTE, "VfArg")->Set(VfArg->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));

		 }*/
		 {
			 using ts = sStruct::Audio0;
			 RenderUtil::md_BufferDesc("audio_inst", BuffDesc, maxaudiosource, sizeof(ts), USAGE_DEFAULT, BIND_UNORDERED_ACCESS|BIND_SHADER_RESOURCE, CPU_ACCESS_NONE);
			 RenderUtil::CreateSetBUf(m_pDevice, rt_.m_pImmediateContext, BuffDesc, &audio_inst, 
				 _SRB, Diligent::SHADER_TYPE_COMPUTE,nullptr,
				 BIND_SHADER_RESOURCE);
		 }
		 auto& pCtx = rt_.m_pImmediateContext;
		 Barriers0[0]= StateTransitionDesc(audio_bufout, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_UNORDERED_ACCESS, STATE_TRANSITION_FLAG_UPDATE_STATE);
		 Barriers0[1]= StateTransitionDesc(v_audio_bufout, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_COPY_SOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE);
		 //Barriers0[1]= StateTransitionDesc(v_audio_bufout, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_COPY_DEST, STATE_TRANSITION_FLAG_UPDATE_STATE);

		 pCtx->TransitionResourceStates(Barriers0.size(), Barriers0.data());
		 /*{
			 using ts = sStruct::Audio0;
			 BuffDesc.Name = "audio_inst_out";
			 BuffDesc.BindFlags = Diligent::BIND_UNORDERED_ACCESS;
			 RenderUtil::SetBUf(m_pDevice, rt_.m_pImmediateContext, BuffDesc, &audio_inst, _SRBfilter);
		 }
		 
		 {
			 using ts = sStruct::VFoutArgs;
			 BuffDesc.Name = "RWVfArg";
			 BuffDesc.BindFlags = Diligent::BIND_UNORDERED_ACCESS;
			 RenderUtil::SetBUf(m_pDevice, rt_.m_pImmediateContext, BuffDesc, &VfArg, _SRBfilter);
		 }
		 {
			 using ts = sStruct::VfProp;
			 BuffDesc.Name = "Vf_Props";
			 BuffDesc.BindFlags = Diligent::BIND_SHADER_RESOURCE;
			 RenderUtil::SetBUf(m_pDevice, rt_.m_pImmediateContext, BuffDesc, &Vf_Props, _SRBfilter);
		 }*/
	 }
	 
	 //void AudioManager::LoadAudios(const fs::path path, const ui64 totalsize, const ui64 Channel, const ui64 samprate)
	 //{
		// daudios.resize(totalsize / 16);
		// auto&& buf0 = daudios;
		// ma_decoder g_pDecoders[1];
		// decoderConfig = ma_decoder_config_init(SAMPLE_FORMAT, Channel, samprate);
		// ma_result result;
		// constexpr int iDecoder = 0;
		// audioinfo ainfo;
		// ainfo.Channel = Channel;
		// ainfo.samprate = samplerate;

		// ui64 frmpross = 0, bkpros = 4096;
		//// for (const auto& entry : fs::directory_iterator(path))
		// {
		//	 result = ma_decoder_init_file_w(path.c_str(), &decoderConfig, &g_pDecoders[iDecoder]);
		//	 //result=
		//	 if (result == MA_SUCCESS) {
		//		 ainfo.offset = frmpross;
		//		 while (true)
		//		 {
		//			 ma_uint32 framesRead = ma_decoder_read_pcm_frames(&g_pDecoders[iDecoder], buf0.data() + frmpross, bkpros);
		//			 frmpross += framesRead;
		//			 if (framesRead < bkpros)
		//				 break;
		//			 if (frmpross + bkpros >= buf0.size())
		//				 buf0.resize(buf0.size() * 2);
		//		 }
		//		 ainfo.size = frmpross - ainfo.offset;
		//		 auto pair = _audiomap.try_emplace(path.c_str(), ainfo);
		//		 /*if (!pair.second) {

		//		 }*/
		//	 }
		//	 else
		//	 {
		//		 LOGINFO("fail to load audio file {0}", path.path());
		//	 }
		//	 ma_decoder_uninit(&g_pDecoders[iDecoder]);
		// }
	 //}
	/*auto AudioManager::getbufpos(const ui64& FrameId) {
		return (FrameId % maxiumLatency) * audiobufsize;
	}*/
	void AudioManager::Update(int audio_insnum) {
		return;
		auto& PrevFrameId = audiol1;
		using namespace Diligent;
		auto&& m_pImmediateContext = _rc.m_pImmediateContext;
		auto&& pCtx = _rc.m_pImmediateContext;
		DispatchComputeAttribs DispatAttribs(1024, 1, 1);
		/*m_pImmediateContext->SetPipelineState(_pbfilter.PSO);
		m_pImmediateContext->CommitShaderResources(_pbfilter.GetShaderResourceBinding(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		m_pImmediateContext->DispatchCompute(DispatAttribs);*/
		//pCtx->TransitionResourceStates(Barriers0.size(), Barriers0.data());
		m_pImmediateContext->SetPipelineState(_pb1.PSO);
		m_pImmediateContext->CommitShaderResources(_pb1.GetShaderResourceBinding(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		constexpr auto bufPos = [&](const auto& freamnum) {return (freamnum % maxiumLatency)* _szCpy; };
		m_pImmediateContext->DispatchCompute(DispatAttribs);
		const auto dist = bufPos(fram);

		Barriers0[0] = StateTransitionDesc(audio_bufout, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_COPY_SOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE);
		//Barriers0[1] = StateTransitionDesc(v_audio_bufout, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_COPY_SOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE);
		Barriers0[1]= StateTransitionDesc(v_audio_bufout, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_COPY_DEST, STATE_TRANSITION_FLAG_UPDATE_STATE);

		pCtx->TransitionResourceStates(Barriers0.size(), Barriers0.data());
		m_pImmediateContext->CopyBuffer(audio_bufout, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY, v_audio_bufout,
			dist, _szCpy, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
		PrevFrameId = _copyfen->GetCompletedValue();
		m_pImmediateContext->EnqueueSignal(_copyfen, fram);
		if constexpr(0) {
			PrevFrameId = fram - maxiumLatency;
			m_pImmediateContext->DeviceWaitForFence(_copyfen, PrevFrameId);
			m_pImmediateContext->WaitForIdle();
		}
		if (fram > 1)
		{
			using ts = v4f;
			Diligent::MapHelper<std::byte> m_MappedData(m_pImmediateContext, v_audio_bufout, MAP_TYPE::MAP_READ, MAP_FLAGS::MAP_FLAG_DO_NOT_WAIT);
			const auto pgdata = &(m_MappedData[bufPos(PrevFrameId)]);
			std::memcpy(audiobuf[PrevFrameId % maxiumLatency], pgdata, _szCpy);
			/*for (size_t i = 0; i < audio_ins; i++)
			{
			assert(m_MappedData[i] == (math::v4f(0, 1, 2, 3) + math::v4f::Ones() * 4 * i) * 500);
			}
			INFO("sum res={0}", m_MappedData[0].transpose());*/
		}
		audiol0 = 0;
		fram++;
	}
	inline void AudioManager::Play_cb(void*& pOutput, ma_uint32 frameCount) {
		//constexpr auto bufPos = [&](const auto& freamnum) {return (freamnum % maxiumLatency) * audiobufsize * 4; };
		//const auto PrevFrameId = fram - maxiumLatency;
		//audiol0 = audiol1 == PrevFrameId ? audiol0 : 0;
		return;
		static auto t1 = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch();
		auto t2 = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch();
		auto dt = (t2 - t1).count() * 0.001;
		t1 = t2;
		auto& PrevFrameId = audiol1;
		if (audiol0 >= audiobufsize || fram<2) {
			LOGINFO("audio null");
			return;
		}
		//audiol1 = PrevFrameId;
		 auto& audioframe = audiobuf[PrevFrameId % maxiumLatency];// audiobuf[PrevFrameId % audiobuf.size()];
		//LOGINFO("audio dt={0} PrevFrameId={1} audiol0={2} audioframe", dt, PrevFrameId, audiol0);
		/*frameCount -= frameCount % 4;
		frameCount /= 4;*/
		size_t totalFramesRead = 0;
		float* pOutputF32 = (float*)pOutput;
		for (size_t i = 0; i < frameCount; i++)
		{
			for (size_t i1 = 0; i1 < CHANNEL_COUNT; i1++)
			{
				pOutputF32[i * CHANNEL_COUNT + i1] = audioframe[audiol0+i];
			}
		}
		audiol0 += frameCount;
		//audioframe++;
	}
	//void AudioManager::Play_cb(ma_device* pDevice, void*& pOutput, ma_uint32 frameCount) {
	//	const auto PrevFrameId = fram - maxiumLatency;
	//	audiol0 = audiol1 == PrevFrameId ? audiol0 : 0;
	//	if (audiol0 >= audiobufsize) {
	//		return;
	//	}
	//	audiol1 = PrevFrameId;
	//	prevaudiofram = PrevFrameId;
	//	auto& audioframe = audiobuf[PrevFrameId];
	//	/*frameCount -= frameCount % 4;
	//	frameCount /= 4;*/
	//	size_t totalFramesRead = 0;
	//	float* pOutputF32 = (float*)pOutput;
	//	//std::memcpy(pOutput,audiobuf[PrevFrameId],)
	//	for (size_t i = 0; i < frameCount; i++)
	//	{
	//		//auto& d=
	//		for (size_t i1 = 0; i1 < CHANNEL_COUNT; i1++)
	//		{
	//			pOutputF32[i * CHANNEL_COUNT + i1] += audioframe[i];
	//		}
	//	}
	//	audiol0 += frameCount;
	//	//audioframe++;
	//}
}