#pragma once
#include"Math1.h"
#include"cnl/scaled_integer.h"
#include"cnl/static_integer.h"
namespace math::Fix {
	using s15_16 = cnl::scaled_integer<int32_t, cnl::power<-24>>;
	inline auto compRadbig(const float& a, const float& b) {
		return fix_s(a) > fix_s(b);
	}
}