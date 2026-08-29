#ifndef GAME_INTERINSERTGIVEBOXEVENTREWARD_H_
#define GAME_INTERINSERTGIVEBOXEVENTREWARD_H_

// HdrCreate2 批（/tmp/missing_h_chunk_ab）：ORIG 有导出符号但仓库无 .h 声明的类。
// 仅声明骨架（不入 CMake，无 cpp 实现）。参数类型按 ORIG demangled 原样；
// 返回类型与 static/成员属性未逐项核实者标【推断】，实现时须以 docs/class_func_reports 为准。
// ORIG 存在 vtable/typeinfo：类含虚函数，析构按 virtual【推断】声明。

#include "TaiwanInterfaceInterDispatcher.h"
class InterInsertGiveBoxEventReward : public TaiwanInterfaceInterDispatcher
{
public:
    // ORIG 0x8162254 _ZN29InterInsertGiveBoxEventRewardC1Ev (size 0x00000024)  报告: docs/class_func_reports/InterInsertGiveBoxEventReward
    InterInsertGiveBoxEventReward() throw();
    // ORIG 0x81622a8 _ZN29InterInsertGiveBoxEventRewardD0Ev (size 0x0000001e)  报告: docs/class_func_reports/InterInsertGiveBoxEventReward
    virtual ~InterInsertGiveBoxEventReward();
    int dispatch_sig(CUser* user, char* buf, int len);
};

#endif  // GAME_INTERINSERTGIVEBOXEVENTREWARD_H_
