#ifndef GAME_LOADSERVICERESTRICTPOLICY_H_
#define GAME_LOADSERVICERESTRICTPOLICY_H_

// HdrCreate2 批（/tmp/missing_h_chunk_ab）：ORIG 有导出符号但仓库无 .h 声明的类。
// 仅声明骨架（不入 CMake，无 cpp 实现）。参数类型按 ORIG demangled 原样；
// 返回类型与 static/成员属性未逐项核实者标【推断】，实现时须以 docs/class_func_reports 为准。
// ORIG 存在 vtable/typeinfo：类含虚函数，析构按 virtual【推断】声明。

#include "TaiwanInterfaceDBDispatcher.h"
#include "TaiwanQueries.h"   // Taiwan::SigLoadRestrictPolicy（sendRestrictData 形参）
class LoadServiceRestrictPolicy : public TaiwanInterfaceDBDispatcher
{
public:
    // ORIG 0x815dff8 _ZN25LoadServiceRestrictPolicyC1Ev (size 0x00000024)  报告: docs/class_func_reports/LoadServiceRestrictPolicy
    LoadServiceRestrictPolicy() throw();
    // ORIG 0x815e04c _ZN25LoadServiceRestrictPolicyD0Ev (size 0x0000001e)  报告: docs/class_func_reports/LoadServiceRestrictPolicy
    virtual ~LoadServiceRestrictPolicy();
    void sendRestrictData(Taiwan::SigLoadRestrictPolicy&);   // ORIG void（汇编无返回设置）
    // ORIG 0x815a8f2 _ZN25LoadServiceRestrictPolicy8dispatchEiiP6Stream (size 0x0000034d)  报告: LoadServiceRestrictPolicy/dispatch.md
    virtual bool dispatch(int code, int uid, Stream* stream);   // ORIG T 0x815a8f2（TaiwanDBDispatcher.cpp）
};

#endif  // GAME_LOADSERVICERESTRICTPOLICY_H_
