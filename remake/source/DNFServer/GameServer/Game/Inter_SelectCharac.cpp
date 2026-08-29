// df_game_r 还原 —— Inter_SelectCharac（ISigDispatcher 派生，仅 4 个 ORIG 符号）。
//   dispatch_sig @ 0x084c528a（size 0xa）：push ebp; mov esp,ebp; mov $0x0,%eax;
//     pop ebp; ret —— 即 return 0;（报告 docs/class_func_reports/Inter_SelectCharac/dispatch_sig.md）。
//   D1/D2 @ 0x084fa236（size 0x30）：设 vptr(0x8c8ff48) → call ISigDispatcherD1Ev →
//     mov $0,%eax; test %al,%al; je（恒假 deleting-check）→ leave; ret —— 用户定义空析构。
//   D0 @ 0x084fa266（size 0x1e）：call D1 → operator delete —— 编译器自动生成。
// 无 C1/C2（隐式 ctor 不产出符号），本 TU 不定义构造函数。

#include "Inter_SelectCharac.h"

Inter_SelectCharac::~Inter_SelectCharac()
{
}

int Inter_SelectCharac::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    return 0;
}
