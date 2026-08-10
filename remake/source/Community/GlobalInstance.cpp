//
// 原始：global_instance 对象（current_time 在前）位于 TU 顶部，生成
// _GLOBAL__I__ZN15global_instance12current_timeE 初始化桩（仅 ios_base::Init）。
//

#include <iostream>

#include "DNFFunctionLib.h"
#include "GlobalInstance.h"

namespace global_instance {
time_t current_time = 0;
bool shutdown = false;
}  // namespace global_instance
