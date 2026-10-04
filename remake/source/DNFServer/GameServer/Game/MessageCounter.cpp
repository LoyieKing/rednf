// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 MessageCounter/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// MessageCounter.cpp —— 按报告 docs/class_func_reports/message_counter__*/*.md 忠实转写。
#include "MessageCounter.h"

#include <new>
#include <string.h>

// [T2b-GI · P338/P327] 补 <iostream> 引入 file-scope 动态初始化 __ioinit，恢复本 TU
// `_GLOBAL__I_` 发射；键名目标 `_ZN15message_counter13CTimeInterval6CreateEv`
// （ORIG 0x857015f）。零语义改动。
#include <iostream>

#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "SigTypes.h"
#include "CSystemTime.h"

namespace message_counter
{

// ---- ORIG 域常量 _ZN15message_counterL13UPDATE_PERIODE（r 0x08c5a78c = 600）----
// gcc 4.4：namespace 内 static const 未使用也发射（内部链接，名含 L）。
static const int UPDATE_PERIOD = 600;

// SIG_INSERT_RESPONSE_TIME（0x7d8）访问视图：
//   +0x00 int  baseSec；+0x04 int count；
//   entries[100] 每 0x14 @+8 + j*0x14：
//     {+0x00 uint id; +0x04 int count; +0x08 int bytesLow;
//      +0x0c int bytesHigh; +0x10 int avgRounded}

// [b28d-SymMsgCtr·检查点18手法] ORIG 无 _ZN15message_counter12_GLOBAL__N_113sig_*
// 等局部符号（inline 在 -O0 仍发射 ⇒ 唯一可行手法是宏）。逐字保留副作用：每形参
// 在展开体中仅出现一次（无双重求值），写序 = 原函数体语句序，无短路逻辑。
// [R29-4] `+4` 字段改用数组下标形（`((int*)(s))[1]`）：ORIG 全部为单条
// disp 折叠 `movl $0x0,0x4(%eax)` / `mov 0x4(%eax),%eax`（0x0856fc94/0x0856fdb0），
// `(s)+4` 形在 4.4 恒多发 `add $0x4,%eax`（4 处调用 ⇒ +4 条）。
#define sig_init_head(s, base) do { \
    reinterpret_cast<MessageCounterSig*>((s))->m_base = (base); \
    reinterpret_cast<MessageCounterSig*>((s))->m_count = 0; } while (0)
#define sig_entry_count(s) (reinterpret_cast<const MessageCounterSig*>((s))->m_count)
#define sig_bump_count(s) (++reinterpret_cast<MessageCounterSig*>((s))->m_count)
// [R29-3] ORIG WriteLog 0x0856fcb5 起：entry 4 个字段是 **4 条独立语句**
// （每条各自 `mov -0x14(%ebp),%eax; mov 0x4(%eax),%ebx` 重读 sig->count ⇒ 4 次），
// bytes 低/高为**一次 64 位写**（`mov 0x10(%eax),%edx; mov 0xc(%eax),%eax;
// mov %eax,0x10(..); mov %edx,0x14(..)`）⇒ 拆 4 条 + 64 位视角。
#pragma pack(push, 1)
struct MessageCounterSigEntry
{
    unsigned int m_id;            // +0x00
    int m_count;                  // +0x04
    unsigned long long m_bytes;   // +0x08
    int m_avg;                    // +0x10
};
#pragma pack(pop)
#pragma pack(push, 1)
struct MessageCounterSig
{
    int m_base;                          // +0x00
    int m_count;                         // +0x04
    MessageCounterSigEntry m_entries[1]; // +0x08 FAM
};
#pragma pack(pop)
#define sig_entry_at(s, slot) (reinterpret_cast<MessageCounterSig*>((s))->m_entries[(slot)])
#define sig_write_entry_id(s, slot, v) (sig_entry_at((s), (slot)).m_id = (v))
#define sig_write_entry_count(s, slot, v) (sig_entry_at((s), (slot)).m_count = (v))
#define sig_write_entry_bytes(s, slot, v) (sig_entry_at((s), (slot)).m_bytes = (v))
#define sig_write_entry_avg(s, slot, v) (sig_entry_at((s), (slot)).m_avg = (v))

#include "MessageCounter/Create.inc"


// ===================== MessageKey =====================

#include "MessageCounter/MessageKey.inc"


// ORIG @0x08570192：先比 id（signed，< 返 1）；相等再比 type（unsigned char，
// jae→0 / < 返 1）；相等再比 size（unsigned short，同上），否则返 0。
bool MessageKey::operator<(const MessageKey& rhs) const
{
    if (m_id < rhs.m_id)
        return true;
    if (m_id == rhs.m_id)
    {
        // uchar/ushort 成员直接比较（ORIG 单 movzbl/movzwl，无 uint 中转槽）；
        // == 守卫形使 4.4 生成 jne 直跳共享 ret0 块（P63 共享出口）。
        if (m_type < rhs.m_type)
            return true;
        if (m_type == rhs.m_type)
        {
            if (m_size < rhs.m_size)
                return true;
        }
    }
    return false;
}
// ===================== CTimeInterval（基类） =====================

// P210：派生 ctor 定义前置于基类 ctor 定义之前——4.4 对「TU 内尚未见定义」的
// callee 不精确继承 outgoing，取保守大帧（ORIG sub $0x18；b28-L11 复验旧
// 「16k+4 不可达」判词：帧差根因是定义顺序而非 pad 量化）。

#include "MessageCounter/CTimeInterval.inc"


#include "MessageCounter/Destory.inc"


#include "MessageCounter/SetLastTime.inc"

#include "MessageCounter/GetTimeInterval.inc"


// ===================== CUnixTimeInterval（派生） =====================

// [R29-2] CUnixTimeInterval() 用户 ctor 已删（声明同步删，P299/CWG434）：
// ORIG nm 中 C1Ev/C2Ev 为 W size 0x1c ⇒ 隐式 ctor；仅此才能让
// `new CUnixTimeInterval()` 的 value-init 前置清零发射（ORIG Create 0x0856f974
// 三 movl $0 + call C1Ev）。隐式 ctor 体 = 调基类 C1Ev + 置派生 vptr，与旧
// 空体用户 ctor 逐条等价。

#include "MessageCounter/SetLastTime_1.inc"
{
    gettimeofday(&m_last, 0);
}
#include "MessageCounter/GetTimeInterval_1.inc"
{
    struct timeval now;
    gettimeofday(&now, 0);

    // [b28d-51] ORIG 0x0857028f 段：x87 全程在栈上（无具名 double 局部往返），
    // fldcw 设 RC=11(截断) 后直接 fistpll 返回，无 `>=0.5` 进位调整。
    return (long long)((double)((int)(now.tv_sec - m_last.tv_sec) * 1000) +
                       ((double)((int)(now.tv_usec - m_last.tv_usec)) / 1000.0));
}
// ===================== CMessageCounter =====================

#include "MessageCounter/CMessageCounter.inc"


#include "MessageCounter/dtor_CMessageCounter.inc"

#include "MessageCounter/IncreaseMessage.inc"


#include "MessageCounter/WriteLog.inc"


}  // namespace message_counter
