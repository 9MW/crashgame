#pragma once

#include "Seri0.h"
namespace  pj2 {
	namespace n_seri {
		
		void DeSeri(serio& s, int mod);
		template<class ...Ts>
		void Seri(Ts&...arg);
	}
}