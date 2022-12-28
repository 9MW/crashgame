#pragma once
#include<imgui.h>
#include <FirstPersonCamera.hpp>
#include <InputController.hpp>
#include"Gameprops0.h"
#include"shpso.h"
#include"Common/InputUtil.h"
namespace pj2{
	using GameProps::Troop;
	enum cmdk {
		Key0, Key1, Key2, Key3, Key4, Key5, Key6, Key7, Key8, Key9,
		A,S,W,D,Q,E, Ctrl,Shift
	};
	/*struct MouseState
	{
		enum BUTTON_FLAGS : Uint8
		{
			BUTTON_FLAG_NONE = 0x00,
			BUTTON_FLAG_LEFT = 0x01,
			BUTTON_FLAG_MIDDLE = 0x02,
			BUTTON_FLAG_RIGHT = 0x04,
			BUTTON_FLAG_WHEEL = 0x08
		};

		Float32      PosX = -1;
		Float32      PosY = -1;
		BUTTON_FLAGS ButtonFlags = BUTTON_FLAG_NONE;
		Float32      WheelDelta = 0;
	};*/
	class inputmanager
	{
		using Entity =GameProps::EntityId;
		struct InputKeyMap
		{
			int
			Unknown = 0,
			MoveLeft=(int)IOin::Key::KEY_A,
			MoveRight = (int)IOin::Key::KEY_D,
			MoveForward = (int)IOin::Key::KEY_W,
			MoveBackward = (int)IOin::Key::KEY_S,
			MoveUp = (int)IOin::Key::KEY_Q,
			MoveDown = (int)IOin::Key::KEY_E,
			Reset,
			ControlDown = (int)IOin::Key::KEY_LEFT_CONTROL,
			ShiftDown= (int)IOin::Key::KEY_LEFT_SHIFT, 
			AltDown,
			ZoomIn,
			ZoomOut,
			TotalKeys;
		};
		using f2 = math::f2;
		using InputController = Diligent::InputController;
	public:
		sStruct::Selects cbsele;
		InputKeyMap kmap;
		shpso::Visible_f* p_vcull;
		computelib::SelectCS* p_picker = NULL;
		mousestate mouseinfo;
		std::array<Troop, 13> tgrop;
		std::array<void*, 8> itf;
		std::bitset<64> kstate;
		std::bitset<512> tkstate;
		int Sele = -1;
		int dfg = -1;
		inputmanager() { pmain = this; }
		void Init();
		~inputmanager() {}
		int writecam(int s);
		int IsKeyDown(const int& k) { return tkstate[k]; }
		template<IOin::Key ...k>
		inline int IsKeyDown() { return (tkstate[(int)k]|...); }
		/*void processinput(InputController& Controller,
			Camera& m_Camera) {
			const ImGuiIO& io = ImGui::GetIO();
			ImGuiContext& g = *ImGui::GetCurrentContext();
		}*/
		void Select(const Entity& id);
		void Select(Utility::span<Entity>id);
		void update();
		void KeyCallback(int key, int scan, int state, int m);
		void MouseButtonCallback(int button, int state, int mods);
		void CursorPosCallback(const f2& pos);
		mousestate& GetMouseState() { return mouseinfo; }
		template<class ...Ts>
		void Cmd(Ts&...arg);
		template<int I,class ...Ts>
		void Cmd(Ts&...arg);
		static inputmanager& main() { return *pmain; }
	private:
		static inline inputmanager* pmain=NULL;
	};

}