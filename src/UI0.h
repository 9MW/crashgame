#pragma once
#include<span>
#include<array>
#include<any>
#include<imgui.h>
#include"cagentity.h"
namespace cage {
	void ComboUI(int& select, std::span<const char*> items);
	inline void SetWindowCenter(const ImVec2& pos) {
		using namespace ImGui;
		auto& io = ImGui::GetIO();
		SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	}
	class UI0
	{
	public:
		std::array<void*, 4> _pdata;
		std::array<UIEvent,4> events;
		void init(std::any);
		void update(int i,std::any);
	};
}
