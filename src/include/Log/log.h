#pragma once
#include<spdlog/spdlog.h>
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h" // must be included
#include<spdlog/fmt/fmt.h>
#define DEBUG
#ifdef DEBUG
#define LOGINFO(...) spdlog::info(__VA_ARGS__)
#define LOGERROR(...) spdlog::error(__VA_ARGS__)
#define ABORDIF(expression, ...) if(!expression) {spdlog::error(__VA_ARGS__); \
            (_wassert(_CRT_WIDE(#expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0); }
#endif // DEBUG

#define INFO(...) spdlog::info(__VA_ARGS__)
#define ERROR(...) spdlog::error(__VA_ARGS__)
namespace profiler{
	//template<typename T>
	//double  timeMeasure(const T fc)
	//{
	//	var t1 = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch();
	//	fc();
	//	var t2 = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch();
	//	var dt = t2 - t1;
	//	double dtd = dt.count() * 0.001;
	//	return dtd;
	//}
}