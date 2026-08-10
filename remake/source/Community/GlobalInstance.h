//
// 原始：global_instance 对象位于独立 TU GlobalInstance.cpp
//（_GLOBAL__I__ZN15global_instance12current_timeE 桩，仅 ios_base::Init）。
//
#pragma once

#include <ctime>

namespace global_instance {
extern bool shutdown;
extern time_t current_time;
}  // namespace global_instance
