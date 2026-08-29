#ifndef GAME_INTER_SELECTCHARAC_H_
#define GAME_INTER_SELECTCHARAC_H_

// df_game_r 还原 —— Inter_SelectCharac（ORIG 单一类：ISigDispatcher 派生）。
// ORIG 符号（df_game_r nm）：
//   T 0x084c528a _ZN18Inter_SelectCharac12dispatch_sigEP5CUserPci（size 0xa）
//   W 0x084fa236 _ZN18Inter_SelectCharacD1Ev / D2Ev（size 0x30）
//   W 0x084fa266 _ZN18Inter_SelectCharacD0Ev（size 0x1e）
// 无 C1/C2 —— 构造函数为编译器隐式生成（不 ODR-use 则不产出符号），本类不声明 ctor。
// vtable 0x8c8ff40：槽序 dispatch_sig(0x84c528a) → D1 → D0，与基类 ISigDispatcher
//   （纯虚 dispatch_sig + 虚析构）完全一致；dispatch_sig 为虚覆写（头旧注"static?"已否定）。
// D1 汇编：设 vptr(0x8c8ff48) → call ISigDispatcherD1Ev → mov $0,%eax; test %al,%al; je
//   （gcc 恒假 deleting-check）→ leave; ret —— 即用户定义空析构。

#include "InterDispatcher.h"   // ISigDispatcher（纯虚 dispatch_sig + 虚析构）

class Inter_SelectCharac : public ISigDispatcher
{
public:
    virtual ~Inter_SelectCharac();
    virtual int dispatch_sig(CUser* user, char* data, int len);
};

#endif  // GAME_INTER_SELECTCHARAC_H_
