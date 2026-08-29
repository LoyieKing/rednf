#ifndef GAME_INTERUPDATEP2PHOLEPUNCHINGSTATE_H_
#define GAME_INTERUPDATEP2PHOLEPUNCHINGSTATE_H_

// HdrCreate2 批（/tmp/missing_h_chunk_ab）：ORIG 有导出符号但仓库无 .h 声明的类。
// 仅声明骨架（不入 CMake，无 cpp 实现）。参数类型按 ORIG demangled 原样；
// 返回类型与 static/成员属性未逐项核实者标【推断】，实现时须以 docs/class_func_reports 为准。
// ORIG 存在 vtable/typeinfo：类含虚函数，析构按 virtual【推断】声明。

#include "TaiwanInterfaceInterDispatcher.h"
class InterUpdateP2pHolePunchingState : public TaiwanInterfaceInterDispatcher
{
public:
    // ORIG 0x81621e2 _ZN31InterUpdateP2pHolePunchingStateC1Ev (size 0x00000024)  报告: docs/class_func_reports/InterUpdateP2pHolePunchingState
    InterUpdateP2pHolePunchingState() throw();
    // ORIG 0x8162236 _ZN31InterUpdateP2pHolePunchingStateD0Ev (size 0x0000001e)  报告: docs/class_func_reports/InterUpdateP2pHolePunchingState
    virtual ~InterUpdateP2pHolePunchingState();
    // ORIG 0x8161952 _ZN31InterUpdateP2pHolePunchingState12dispatch_sigEP5CUserPci (size 0x00000137)：
    // 基类虚 dispatch_sig 的 override。
    virtual int dispatch_sig(CUser*, char*, int);
};

#endif  // GAME_INTERUPDATEP2PHOLEPUNCHINGSTATE_H_
