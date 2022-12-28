#pragma once

namespace pj2::n_seri {
	struct Serialzable {
	};
	template<int i = -1>
	struct DependencyLevel :Serialzable {
		static constexpr int Level = i;
	};
}