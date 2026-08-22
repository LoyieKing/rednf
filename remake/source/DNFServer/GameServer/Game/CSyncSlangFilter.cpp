#include "CSyncSlangFilter.h"

// ORIG 无此符号，工程自造桥接 _ZN16CSyncSlangFilter6FilterERKSs（GameWorld.cpp 经 bridge 调用）。
// 仅定义 Filter，其余方法在 GlobalData.cpp 定义。
void CSyncSlangFilter::Filter(const std::string& str)
{
    (void)str;
}