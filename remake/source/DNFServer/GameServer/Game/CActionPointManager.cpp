// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CActionPointManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ APSystem::CActionPointManager 实现（批3：从 GameStubs.cpp 迁移）。
// 类声明位于 CUser.h（APSystem::CActionPointManager）。本 TU 是这些符号的唯一权威定义。
// ORIG：GM_Set @0x8121b3e（T）、Reset @0x8122af4（W）、GetActionPoint @0x8694bfc（W）。
#include "CUser.h"
#include <string.h>

namespace APSystem
{

// _ActionGroupIndex / CUserProc 权威声明见 CUserProc.h（本 TU 本地镜像已删）。

// ORIG 0x8694bfc（W，内联）：mov 0x8(%ebp),%eax; add $0x8,%eax; ret。
// 返回嵌入的 _SIG_LOAD_ACTION_POINT 首地址；具体返回类型仍按现有 ABI
// 以字节指针暴露，避免把 packed 包误当成普通对齐对象。
#include "CActionPointManager/GetActionPoint.inc"
{
    return reinterpret_cast<unsigned char*>(
        const_cast<_SIG_LOAD_ACTION_POINT*>(&m_sig));
}

// ORIG 0x8121b3e：m_flag00=1；m_sig.m_apSum=0；m_prevApSum=0；
// 先把所有已领条目的五个日期槽取负标记；再按行/槽扫描，在行动点总数
// 未达目标 value 前逐项恢复、累加并发包通知。_ActionPoint 已按 ORIG
// 0xb 字节 packed，具名成员访问仍保持 +0x00/+0x02/+0x06 的真实布局。
#include "CActionPointManager/GM_Set.inc"
{
    // [2026-09-11 L8/N4Free191to210.S6 复核] 本件由 DIFF 141/133 推至 **141/142**
    //（gap −1，逐条 reloc 归一后仅剩 3 处寄存器选择）：
    // 落地 = ① 外层 for 去掉 `m_sig.m_apSum < value`（ORIG 0x08121cfa 外层尾只有
    //   `cmpl $0x12b`+setle/test/jne，限额判据只在内层 while 头）；② 循环 1/2 的取负
    //   去掉 `static_cast<unsigned char>`（ORIG 0x08121b9d 为 movzbl→neg→mov %cl，
    //   无 movsbl）；③ 删 entry/restored/group 具名局部（ORIG 帧无 -0x1d/-0xe/-0xd 槽）
    //   ⇒ 帧 0x58→0x44、push %ebx 复现（两侧 `add $0x44,%esp; pop %ebx`）。
    // 残差（N7 封顶，两次实测）：① 两处 `mov %edx,%ecx; neg %ecx`（ORIG）
    //   vs `neg %edx; mov %edx,%ecx`；② groupIndex 直载 %ecx（ORIG）vs 经 %eax 中转。
    //   已试并回滚：C 风格 cast 替 static_cast（零变化）、消具名局部以外的寄存器
    //   搬运写法（零变化）⇒ 4.4 的寄存器分配产物，源形不可控。
    m_flag00 = 1;
    m_sig.m_apSum = 0;
    m_prevApSum = 0;
    for (int row = 0; row <= 299; ++row)
    {
        _ActionPoint& rec = m_sig.m_actionPoints[row];
        for (int slot = 0; slot <= 4; ++slot)
        {
            // ORIG 0x08121b8e/0x08121b95：movzbl 0x6(rec,slot); test %al,%al; jle ——
            // 判据直接落在内存读上（无 entry 具名槽，ORIG 帧 -0x1d 不存在）。
            if (static_cast<signed char>(rec.m_days[slot]) > 0)
            {
                // ORIG 0x08121b9d..0x08121bac：movzbl → neg → mov %cl（无 movsbl）⇒
                // 取负作用于 unsigned char 提升后的 int。
                rec.m_days[slot] = -rec.m_days[slot];
            }
        }
    }
    // ORIG 0x08121cfa：外层尾只有 cmpl $0x12b,row + setle/test/jne ——
    // 外层 for 不带 apSum < value 条件；限额判据仅在内层 while 头。
    for (int row = 0; row <= 299; ++row)
    {
        _ActionPoint& rec = m_sig.m_actionPoints[row];
        for (int slot = 0; slot <= 4 && m_sig.m_apSum < value; ++slot)
        {
            // ORIG 0x08121c06/0x08121c0d：movzbl 0x6(rec,slot); test %al,%al; jns。
            if (static_cast<signed char>(rec.m_days[slot]) < 0)
            {
                m_prevApSum = m_sig.m_apSum;
                rec.m_days[slot] = -rec.m_days[slot];
                m_sig.m_apSum += static_cast<signed char>(rec.m_days[slot]);
                // ORIG 0x08121c55..0x08121c83：实参按 m_sig.m_actionPoints[row]
                // 重算地址（this+row*0xb+0x15/+0x17），非复用 rec 指针；
                // 第二个索引重载在 ORIG 落到 callee-saved ebx（push/pop %ebx）。
                CUserProc::SendActionClear(
                    user, m_sig.m_actionPoints[row].m_actionNo,
                    static_cast<_ActionGroupIndex>(
                        m_sig.m_actionPoints[row].m_groupIndex),
                    true, ENUM_PACKETCLASS_0);
                CUserProc::CheckAndSendToMessageAllUser(
                    m_prevApSum, m_sig.m_apSum, user.getCurCharacName());
            }
        }
    }
}

// ORIG 0x08122af4（W）：m_flag00=0；m_prevApSum=0；
// memset(&m_sig, 0, sizeof(_SIG_LOAD_ACTION_POINT))。无负标记扫描循环。
#include "CActionPointManager/Reset.inc"
{
    m_flag00 = 0;
    m_prevApSum = 0;
    memset(&m_sig, 0, sizeof(m_sig));
}

} // namespace APSystem
