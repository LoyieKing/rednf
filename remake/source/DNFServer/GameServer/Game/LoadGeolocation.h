#ifndef GAME_LOADGEOLOCATION_H_
#define GAME_LOADGEOLOCATION_H_

// HdrCreate2 批（/tmp/missing_h_chunk_ab）：ORIG 有导出符号但仓库无 .h 声明的类。
// 仅声明骨架（不入 CMake，无 cpp 实现）。参数类型按 ORIG demangled 原样；
// 返回类型与 static/成员属性未逐项核实者标【推断】，实现时须以 docs/class_func_reports 为准。
// ORIG 存在 vtable/typeinfo：类含虚函数，析构按 virtual【推断】声明。

#include "TaiwanInterfaceDBDispatcher.h"
#include "TaiwanQueries.h"   // Taiwan::SigLoadGeolocation（sendCountryData 形参）
class LoadGeolocation : public TaiwanInterfaceDBDispatcher
{
public:
    // ORIG 0x815e0dc _ZN15LoadGeolocationC1Ev (size 0x00000024)  报告: docs/class_func_reports/LoadGeolocation
    LoadGeolocation() throw();
    // ORIG 0x815e130 _ZN15LoadGeolocationD0Ev (size 0x0000001e)  报告: docs/class_func_reports/LoadGeolocation
    virtual ~LoadGeolocation();
    void sendCountryData(Taiwan::SigLoadGeolocation&);   // ORIG void（汇编无返回设置）
    // ORIG 0x815b010 _ZN15LoadGeolocation8dispatchEiiP6Stream (size 0x00000256)  报告: LoadGeolocation/dispatch.md
    virtual bool dispatch(int code, int uid, Stream* stream);   // ORIG T 0x815b010（TaiwanDBDispatcher.cpp）
};

#endif  // GAME_LOADGEOLOCATION_H_
