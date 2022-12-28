#pragma once
#include"Common/Math1.h"
namespace pj2::geomatrys {
	template<class T>
	void circle(int vertnum,float r,Utility::span<T> circlevtx) {

		for (int ii = 0; ii < vertnum; ii++) {
			float theta = 2.0f * 3.1415926f * float(ii) / float(vertnum);//get the current angle 
			
			auto& vt = circlevtx[ii];
			vt.head<2>()= { r * cosf(theta), r * sinf(theta) };
		}
}
}