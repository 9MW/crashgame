#pragma once
#include<span>
#include<array>
namespace cage {
	void ComboUI(int& select, std::span<const char*> items);
	class UI0
	{
		std::array<void*, 8> _pdata;
		void init(void*);
		void update(int i,void*);
	};
}
