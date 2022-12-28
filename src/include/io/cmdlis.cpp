#include"commandin.h"
#include"entitys/Commander.h"
#include "GLFW/glfw3.h"
#include"GpropExt.h"
#include"imgui.h"
namespace pj2 {
	using t_sinout = sStruct::seleout;
	inputmanager* pi;
	struct cmdstate
	{
		int sele;
	};
	enum enueve
	{
		ray, rect, attack, follow
	};
	struct MouseState
	{
		math::i64 time;
		math::f2 mousepos;
		int key;  int state;
		MouseState() {
			mousepos.setZero();
			time = 0;
		}
	};
	struct 	t_evt
	{
		int tid;
		enueve evt;
		//eveenu len;
		int64_t fs;
		/*bool operator==(const int& i) {
			return i == tid;
		}*/
		bool operator==(const enueve& i) {
			return i == evt;
		}
	};
	std::queue<t_evt> evquecmd;
	int uiunit = 0;
	MouseState lastdown, cst, prelease;
	int cam = 0;
	int inputmanager::writecam(int s) {
		cam = s;
		return 0;
	}
	struct cmdevent {
		int select, deselect, attack, stop;
	};
	using namespace GameProps;
	void inputmanager::KeyCallback(int key, int scan, int state, int modf)
	{
		using namespace IOin;
		tkstate[key] = state == (int)KeyState::Repeat ? 1 : state;
	}
	struct cvetb {
		int beg, num, bid, beid;
		//std::vector<int> insid;
	};
	constexpr auto covertcoord = [](const auto& a, auto& outv, const float w, const float h) {
		//outv=
		outv[0] = 2 * (a[0] / w) - 1;
		//outv[1]= 2 * (a[1] / h) - 1;
		outv[1] = 1 - 2 * (a[1] / h);
	};
	std::array<int, 32>keymap;
	int64_t seleframe = -1, evframe;
	GameProps::Cmd0 _ccmd;
	//using namespace GameProps;
	void inputmanager::Init() {
		pi = this;
		mouseinfo.key = 0;
		seleframe = -1; pmain = pmain ? pmain : this;
		cbsele.maxin.setConstant(0);
	}
	constexpr int mmstate = 0xf;
	constexpr int mnumbit = 4;
	inline bool Click(const f2 a, const f2 b) {
		return (a - b).squaredNorm() < 16;
	}
	void inputmanager::MouseButtonCallback(int button, int state, int mods) {
		//using BUTTON_FLAGS = mousestate::BUTTON_FLAGS;
		LOGINFO("MouseButtonCallback {0} {1} {2}", button, state, mods);
		using namespace IOin;

		auto butis = [&button](const IOin::Key k) {return button == (int)k; };
		auto stateis = [&state](const IOin::KeyState& k) {return state == (int)k; };
		using namespace GameProps::ENU;
		using namespace IOin;
		static std::vector<cvetb> odesc(20);
		static std::vector<int> isid(512);
		auto& mstate = mouseinfo.state;
		odesc.clear();
		isid.clear();
		const KeyMouse& kb = (KeyMouse&)button;
		const KeyState& ks = (KeyState&)state;
		int cvr;//can't big than 8
		if (!stateis(KeyState::Repeat)) {
			switch (kb) {
			case KeyMouse::LEFT:
				cvr = mousestate::BUTTON_FLAG_LEFT;
				break;
			case KeyMouse::RIGHT:
				cvr = mousestate::BUTTON_FLAG_RIGHT;
				break;
			case KeyMouse::MIDDLE:
				cvr = mousestate::BUTTON_FLAG_MIDDLE;
				break;
			default:
				std::abort();
			}
			mouseinfo.key |= cvr;
			if (stateis(KeyState::Press)) {
				mouseinfo.key |= cvr;
				mouseinfo.state |= cvr;
			}
			else {
				assert((mouseinfo.key & cvr));
				mouseinfo.key &= ~cvr;
				mouseinfo.state &= ~cvr;
			}

			const auto& mp = ImGui::GetMousePos();
			//Camera::Main().getRay({ mp.x,mp.y }, v1, v2);
		}
	}
	void inputmanager::CursorPosCallback(const f2& pos) {
		//LOGINFO("cursor position={0}", pos.transpose());
		mouseinfo.CursorPos = pos;
	}

	/*void processinput(InputController& Controller,
	Camera& m_Camera) {
	const ImGuiIO& io = ImGui::GetIO();
	ImGuiContext& g = *ImGui::GetCurrentContext();
	}*/

	void inputmanager::Select(const Entity& id) {
		Sele = 0;
		auto& stroop = tgrop[Sele];
		stroop.units.clear();
		stroop.units.emplace_back(id);
	}
	void inputmanager::Select(Utility::span<Entity> id) {
		//not finished
		assert(0);
	}
	static int cmdid = 0;
	using tfunc = int(*)(void*);
	using rayreg = std::tuple< tfunc, void*, int>;
	vector<rayreg> Listener;
	rayreg ls0;
	template<>
	void inputmanager::Cmd<1, rayreg>(rayreg& rg) {
		std::get<2>(rg) = Listener.size();
		ls0 = rg;
		//Listener.push(rg);
	}
	template<>
	void inputmanager::Cmd<-1, int>(int& rg) {
		std::get<0>(ls0) = NULL;
	}
	void inputmanager::update() {
		static int capture = 1;
	}
}
