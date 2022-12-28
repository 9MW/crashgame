#pragma once
namespace pj2::Graphic {
	using uint = unsigned int;
	struct stateanim
	{
		uint beg, size, fevt;
		float speed = 1, length = 0, pp, p;
	};
}