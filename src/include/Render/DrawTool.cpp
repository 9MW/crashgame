#include "DrawTool.h"
#include "LineRender.h"
namespace  pj2::n_Render {
	template<>
	void execmd<0>(std::array< v4f,4>& linepara)
	{
		LineRender::main()->NewLine(linepara.data());
	}
	void execmd(void*)
	{
	}
}
