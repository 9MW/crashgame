#pragma once
#include<limits>
namespace pj2::math {
	template<class T>
	constexpr T max() { return std::numeric_limits<T>::max(); }
}