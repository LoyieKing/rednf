//
// 原始：global_function::sleep 位于独立 TU（首函数 sleep），生成
// _GLOBAL__I__ZN15global_function5sleepEi 初始化桩（仅 ios_base::Init）。
//

#include <iostream>

#include <unistd.h>

#include "DNFFunctionLib.h"
#include "GlobalFunction.h"

namespace global_function {
void sleep(int ms) {
    usleep(ms * 1000);
}
}  // namespace global_function
