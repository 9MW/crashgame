#pragma once
#include "Comps1.h"
//#include"AllType.h"
//#include<Windows.h>
//#include <fcntl.h>
#include <corecrt_io.h>
#include "Log/log.h"
#include"json.h"
//inline auto& unit0::Weapon(Objs::Equips& eqp)
//{
//	// TODO: insert return statement here
//}
namespace pj2::n_weapon {
	using json = nlohmann::json;
	namespace gunpropname {
		const std::string
			gun_name = "gun_name",
			shotAudio = "shotAudio",
			reloadaud = "reloadaud",
			finishloadaud = "finishloadaud",
			description = "descption",
			reload_time = "reload_time",
			bulleti = "bullet id",
			capacity = "capacity",
			firevel = "attackvel";
	}
	void gun0::Seri(void* js) {
		json& j = *(json*)js;
		auto& g = *this;
		j.push_back({
			{gunpropname::reload_time,math::Fix::toFloat(g.treload)},
			{gunpropname::bulleti,g.bullet.d},
			{gunpropname::capacity,g.cap},
			{gunpropname::firevel,1 }
			});
		bulletinfo bullet;
		//int treload, audiosrc, audiolength;
		j = json{ {"treload",treload} };
	}
	using namespace math::Fix;
	void guns0::Seri(void* js) {
		json& j = *(json*)js;
		auto length = size();
		if (length == 0) {
			std::string se="unset";
			j.push_back({
				{gunpropname::gun_name,se},
				{gunpropname::shotAudio,se},
				{gunpropname::reloadaud,se},
				{gunpropname::finishloadaud,se},
				{gunpropname::description,se},
				{gunpropname::reload_time,FP_NAN},
				{gunpropname::bulleti,-1 },
				{gunpropname::capacity,-1 },
				{gunpropname::firevel,1 }
				});
		}
		for (size_t i = 0; i < length; i++)
		{
			auto&& [g, wr] = this->getcmps < gun0, wepresources >(i);
			j.push_back({
				{gunpropname::gun_name,wr.name},
				{gunpropname::shotAudio,wr.shotAudio},
				{gunpropname::reloadaud,wr.reloadaud},
				{gunpropname::finishloadaud,wr.finishloadaud},
				{gunpropname::description,wr.desc},
				{gunpropname::reload_time,math::Fix::toFloat(g.treload)},
				{gunpropname::bulleti,g.bullet.d},
				{gunpropname::capacity,g.cap},
				{gunpropname::firevel,1 }
				});
		}
	}

	void guns0::DeSeri(void* js)
	{
		json& j = *(json*)js;
		using namespace gunpropname;
		this->resize(j.size());
		for (size_t i = 1; i < j.size(); i++)
		{
			auto&& [g, wr] = this->getcmps <gun0, wepresources>(i);
			auto& ji = j[i];
			int x = 0;
			g.cap = ji[gunpropname::capacity].get<int>();
			g.treload =toFix(ji["reload_time"].get<float>());
			if(!ji[gunpropname::firevel].is_null())
			wr.attackvel =(ji[gunpropname::firevel].get<float>());
			wr.name = ji["gun_name"].get<string>();
			wr.shotAudio = ji["shotAudio"].get<string>();
			wr.reloadaud = ji["reloadaud"].get<string>();
			wr.finishloadaud = ji["finishloadaud"].get<string>();
			wr.desc = ji[description].is_null()?"": ji[description];
		}
	}

}
namespace pj2::Objs {

}
//using namespace pj2;
//static const WORD MAX_CONSOLE_LINES = 10000;
//int WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int cmdShow) {
//
//	int hConHandle;
//	long lStdHandle;
//	CONSOLE_SCREEN_BUFFER_INFO coninfo;
//	FILE* fp;
//
//	// allocate a console for this app
//	AllocConsole();
//
//	// set the screen buffer to be big enough to let us scroll text
//	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
//	coninfo.dwSize.Y = MAX_CONSOLE_LINES;
//	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
//
//	// redirect unbuffered STDOUT to the console
//	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
//	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
//	fp = _fdopen(hConHandle, "w");
//	*stdout = *fp;
//	setvbuf(stdout, NULL, _IONBF, 0);
//
//	// redirect unbuffered STDIN to the console
//	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
//	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
//	fp = _fdopen(hConHandle, "r");
//	*stdin = *fp;
//	setvbuf(stdin, NULL, _IONBF, 0);
//
//	// redirect unbuffered STDERR to the console
//	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
//	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
//	fp = _fdopen(hConHandle, "w");
//	*stderr = *fp;
//	setvbuf(stderr, NULL, _IONBF, 0);
//
//	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
//	// point to console as well
//	std::ios::sync_with_stdio();
//	spdlog::shutdown();
//	spdlog::set_default_logger(spdlog::stdout_color_mt("console"));
//	LOGINFO("sccs");
//	
//	using RGS = n_Registry::Registry;
//	ECS::visit_all_convertible<n_Registry::Registry, n_BasObj::Moveobj>([&](auto I) {
//		using cc = RGS::template t<I>;
//		LOGINFO("I={0} {1}", typeid(cc).name(),I);// , ecl12::template elei<I>::jo());
//		});
//	pj2::Objs::main();
//	/*ECS::visit_all<n_Registry::Registry>([&](auto I) {
//					LOGINFO("{0}", typeid(I).name());
//					});*/
//	return 0;
//}

void pj2::n_BasObj::Commander::SortId() {
	auto& e = getcomp<EntityId>();
	std::sort( e.begin(), e.end());
}
