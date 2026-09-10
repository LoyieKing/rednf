// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 TimerQueue/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r TimerQueue 实现（ORIG ctor 0x80f63ac / dtor 0x82a42b8 /
// InsertTimer 0x8630cec / GetTimerMess 0x8630ecc，AE 口径还原）。
#include "TimerQueue.h"
#include "CEnvironment.h"

#include <stdio.h>
#include <iostream>   // std::ios_base::Init __ioinit（ORIG GI 键名 _ZN10TimerQueue11InsertTimerER10TimerEntry 的前提，P338）
#include <string>

// 全局时钟对象属于 GlobalData（ORIG BSS 0x941f714），真实定义在其它 TU。
class CSystemTime
{
public:
    unsigned int getCurTickCount();
};

struct GlobalData
{
    static CSystemTime s_systemTime_;
};
// [P327 GI 键名] ORIG 本 TU `_GLOBAL__I_` 键名 = _ZN10TimerQueue11InsertTimerER10TimerEntry
// ⇒ 该重载定义须为本 TU 首个强符号（此前 TimerEntry::TimerEntry 定义在前 ⇒ 键名错为
// _GLOBAL__I__ZN10TimerEntryC2Ev）。纯前置函数定义，符号集合/构造序零变化。
#include "TimerQueue/InsertTimer.inc"
}

// TimerEntry（ORIG 0x8630e60 族调用方无 EH 清理块，throw() 对齐）
#include "TimerQueue/TimerEntry.inc"
}
bool TimerEntry::operator<(const TimerEntry& other) const throw()
{
    // b28d-49 @0x80f6364：ORIG 23 条 —— this.lo→%ecx、this.hi→%ebx（callee-saved，
    // push esi/ebx），hi 有符号 jg/jl、lo 无符号 jae，结果经 %esi 归一返回。
    // 源码形 = 纯 register 存储类（P231，非 asm 绑定）+ 头内 32 位双字视图；
    // 残差（%ecx/%esi 角色互换、比较方向反演）属 4.1 预载操作数 vs 4.4 惰性装载，
    // N7/N18a 封顶（判词见函数头）。
    // b28d-49 判词：N 封顶（N7+N18a）。ORIG 预载 this.lo→%ecx 跨两次 hi 比较、
    // 结果单变量 %esi（`mov $0x1,%esi` … `mov %esi,%eax`）；4.4 恒惰性装载且
    // 只用 callee-saved（探针 h1-h4 四形：%esi/%ebx/%edi 角色互换 + 比较方向反演）。
    // 语义已对齐（hi 有符号、lo 无符号；旧 long long 版 hi 无符号 = 真语义偏差，已修）。
    // b28d-solo-TimerQueue 判词：N 封顶（N7+N18a）。本轮七形实测（fndiff ndiff，
    // ORIG 23 条）：本形(==优先) 24（最小）/ 原 if-return 27 / 三目链 32 /
    // register bool ret 30 / 3×register 32 / 取反尾 27 / 交换 if 序 27 —— 全形非 0。
    // ORIG「other 一次装载(hi→%edx,lo→%eax) + this.lo→%ecx + 结果 %esi 预置
    // + jg/jl/jae 单变量归一」为 4.1 特有寄存器分配；P248 三档旋钮不适用
    //（无字段可换型、返回 bool 已同型）⇒ 语义已对齐，封顶。
    register unsigned int thisLo = words.lo;
    register int thisHi = words.hi;
    if (thisHi == other.words.hi)
        return thisLo >= other.words.lo;
    return thisHi > other.words.hi;
}

#include "TimerQueue/TimerQueue.inc"
}

#include "TimerQueue/dtor_TimerQueue.inc"
}

#include "TimerQueue/InsertTimer_1.inc"
}

#include "TimerQueue/InsertTimerInMilisecond.inc"
}

#include "TimerQueue/GetTimerMess.inc"
}

#include "TimerQueue/GetQueueSize.inc"
}

#include "TimerQueue/write_timer_queue_log.inc"
}

// ---- 全局单例访问器 ----
// ORIG 0x80f647c W（_Z12G_TimerQueuev）：GlobalInstance<TimerQueue>::inst_ptr()
// （create 惰性 new 并缓存至 m_p，ORIG m_p 位于 BSS 0x93facc4）。
TimerQueue* G_TimerQueue()
{
    return GlobalInstance<TimerQueue>::inst_ptr();
}
template class GlobalInstance<TimerQueue>;
