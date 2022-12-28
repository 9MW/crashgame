#pragma once
#include "Common\Math1.h"
//units as milliseconds
constexpr double logicstep = 20;
constexpr int MaxSelect = 2000;
constexpr float MaxBSV = 100;
namespace pj2 {
	constexpr int MaxCmdCache = 16;
	constexpr int logicframerate = 1000/ logicstep;
	namespace Audio {
#define DEVICE_CHANNELS         2
	constexpr int samplerate = 44100;
	//constexpr int Perframe = 44100;

	}
	namespace GloProp
	{
		struct Prop {
			math::mtransform pretrans;
			int64_t FrameId = 0;
			int maxSelect=200;
			math::ui64 maxcullnum = 2048, maxinstancenum=1024*10,MaxTypenum=256;

			float dt;
			static inline auto& Get() {
				static Prop p;
				return p;
			}
		};
		static  int64_t FrameId = 0;
		static inline auto& Get() {
			return Prop::Get();
		}
	};
}