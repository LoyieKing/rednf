#ifndef GAME_INTERSELECTMOBILEAUTHREWARD_H_
#define GAME_INTERSELECTMOBILEAUTHREWARD_H_

// HdrCreate2 批（/tmp/missing_h_chunk_ab）：ORIG 有导出符号但仓库无 .h 声明的类。
// 仅声明骨架（不入 CMake，无 cpp 实现）。参数类型按 ORIG demangled 原样；
// 返回类型与 static/成员属性未逐项核实者标【推断】，实现时须以 docs/class_func_reports 为准。
// ORIG 存在 vtable/typeinfo：类含虚函数，析构按 virtual【推断】声明。

#include "TaiwanInterfaceInterDispatcher.h"
class InterSelectMobileAuthReward : public TaiwanInterfaceInterDispatcher
{
public:
    // ORIG makeRequest 0x081615ae 写 +0x0(uint m_id)/+0x4(bool rewarded)；sizeof=8。
    struct Msg
    {
        unsigned int m_id;       // +0x00
        bool m_rewarded;         // +0x04
    };

    // ORIG 0x81620fe _ZN27InterSelectMobileAuthRewardC1Ev (size 0x00000024)  报告: docs/class_func_reports/InterSelectMobileAuthReward
    InterSelectMobileAuthReward() throw();
    // ORIG 0x8162152 _ZN27InterSelectMobileAuthRewardD0Ev (size 0x0000001e)  报告: docs/class_func_reports/InterSelectMobileAuthReward
    virtual ~InterSelectMobileAuthReward();
    // ORIG 0x81615ae _ZN27InterSelectMobileAuthReward11makeRequestEijb (size 0x00000120)  报告: InterSelectMobileAuthReward/makeRequest.md  【推断:static?】
    static void makeRequest(int, unsigned int, bool);  // ORIG 0x81615ae（无 this 压参 → static；Ghidra C void）
    // ORIG 0x816132a _ZN27InterSelectMobileAuthReward12dispatch_sigEP5CUserPci (size 0x00000283)：
    // 基类虚 dispatch_sig 的 override（this 未用但 virtual 槽对齐）。
    virtual int dispatch_sig(CUser*, char*, int);
};

#endif  // GAME_INTERSELECTMOBILEAUTHREWARD_H_
