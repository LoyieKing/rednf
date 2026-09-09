// df_game_r 还原 —— stQuestTriggerState_t 独立 TU（batch 迁移自 GameStubs.cpp）。
// 权威符号（ORIG df_game_r）：
//   stQuestTriggerState_t::stQuestTriggerState_t()             W @0x0836ddac
//   stQuestTriggerState_t::operator=(int const&)               W @0x086ad56c
//   stQuestTriggerState_t::incTirggerState(char)               W @0x086ad602
//   stQuestTriggerState_t::subTriggerState(char)               W @0x086ad6ea
//   stQuestTriggerState_t::getTrigger()                        W @0x0836dd68
//
// 语义：3 个 9 位触发值（mask 0x1ff）打包于 +0x00/+0x02/+0x04（对象 8 字节）。
//   operator= 由 int 解包三字段；getTrigger 打包三字段为 int；
//   incTirggerState 按 v 的位（0x10/0x20/0x40）选字段自增；
//   subTriggerState v==0 对字段0 裸自减，否则按位选字段自减（mod 512）。
//   ORIG 对每字段写回时保留高 7 位（movzwl 旧值 and 0xfe00；or 新低 9 位后写回），
//   重建需保持同款位保留语义。
//
// [符号绑定判据 2026-09-10] 上述符号 ORIG 全为 W（类内 inline），我方原为 T；
//   已在 QuestTriggerState.h 声明处加 __attribute__((weak)) ⇒ 本 TU 发射 W 实体
//   （尺寸逐位不变：getTrigger 0x46 / inc 0xe8 / sub 0xfa / C1C2 0x15 / aSERKi 0x7c）。
// [跨版本残差 2026-09-10 复核] 逐 call*/jcc 核对：本 TU 无间接调用、5 函数 jcc
//   0 mismatch。operator= gap=5（45 vs 40）：用 c5root 4.1.2 探针穷举 13 种源码形
//   （cast 组合/操作数序/掩码序/成员类型）无一复现 ORIG 的「3×`and $0x1ff,%ax`
//   + 新值先入 %ecx 再读旧值」形 ⇒ 判 N（跨版本窄类型 lowering + 写回求值序）。
//   inc/sub/getTrigger 的 `and $0x1ff,%ax; movzwl` ↔ `movzwl; and $0x1ff,%eax`
//   顺序差同为该族，条数全等（73/73、81/81、23/23）。

// [§4.4 合并 2026-09-10] 类型唯一定义点 = QuestTriggerState.h（本 TU 曾重复定义
// class 镜像，与 Quest.h 的 struct 逐 token 不同：方法声明序异、无 setTrigger ⇒ ODR
// 违规）。成员布局（4×short / 8 字节）与原权威定义逐字段一致，合并零 ABI 变化。
#include "QuestTriggerState.h"

stQuestTriggerState_t::stQuestTriggerState_t()
{
    // ORIG：movl $0x0,(%eax); movl $0x0,0x4(%eax) —— 按 8 字节（2×int）整块清零
    *(long long*)this = 0;
}

stQuestTriggerState_t& stQuestTriggerState_t::operator=(int const& v)
{
    // ORIG：先 movl $0,(%eax); movl $0,0x4(%eax) 清 8 字节再写三字段（全重置语义）
    // [b28d-lane15] 去具名 int* p 局部（ORIG 无槽，直接 this：`movl $0,(%eax)` +
    // `movl $0,0x4(%eax)` 单 disp 形，P0/P183）。
    // [b28d-lane15 判词·N] 残差两类：①窄类型 `and $0x1ff,%ax`（16 位）vs 4.4
    // `and $0x1ff,%eax`（同 getTrigger，跨版本 lowering）；②写回求值序 ORIG 先算
    // 新值入 %ecx 再读旧值（0x086ad5xx `mov this,%edx; mov %eax,%ecx; and $0x1ff,%cx;
    // movzwl (%edx),%eax; and $0xfe00,%ax; or %ecx,%eax`），4.4 恒先读旧值。
    // 45 vs 40，语义等价，封顶。
    *(long long*)this = 0;
    m_trigger0 = (short)(((unsigned short)m_trigger0 & 0xfe00) | (v & 0x1ff));
    m_trigger1 = (short)(((unsigned short)m_trigger1 & 0xfe00) | ((v >> 9) & 0x1ff));
    m_trigger2 = (short)(((unsigned short)m_trigger2 & 0xfe00) | ((v >> 18) & 0x1ff));
    return *this;
}

void stQuestTriggerState_t::incTirggerState(char v)
{
    // [b28d-lane15] 去具名 short cur 局部（ORIG 0x086ad602 无槽：`movzwl %ax,%eax;
    // cmp $0x3fe,%eax` 32 位直比，P0/P183 表达式内联重算）。
    // [b28d-lane15 判词·N] 73=73；残差仅窄类型 and 宽度（`and $0x1ff,%ax` vs %eax）
    // 与写回求值序（ORIG 新值先入 %ecx，4.4 先读旧值）——同 operator= 两族，封顶。
    if (v & 0x10)
    {
        if (((unsigned short)m_trigger0 & 0x1ff) <= 0x3fe)
            m_trigger0 = (short)(((unsigned short)m_trigger0 & 0xfe00) |
                                 ((((unsigned short)m_trigger0 & 0x1ff) + 1) & 0x1ff));
    }
    if (v & 0x20)
    {
        if (((unsigned short)m_trigger1 & 0x1ff) <= 0x3fe)
            m_trigger1 = (short)(((unsigned short)m_trigger1 & 0xfe00) |
                                 ((((unsigned short)m_trigger1 & 0x1ff) + 1) & 0x1ff));
    }
    if (v & 0x40)
    {
        if (((unsigned short)m_trigger2 & 0x1ff) <= 0x3fe)
            m_trigger2 = (short)(((unsigned short)m_trigger2 & 0xfe00) |
                                 ((((unsigned short)m_trigger2 & 0x1ff) + 1) & 0x1ff));
    }
}

void stQuestTriggerState_t::subTriggerState(char v)
{
    if (v == 0)
    {
        // ORIG：`mov (%eax),%eax; lea -1(%eax),%edx; mov %edx,(%eax)` —— 对 +0x00 的
        // 4 字节值（m_trigger0/m_trigger1 合并）裸自减 1。
        // [b28d-lane15] 去具名 lo 局部（ORIG 无槽）。
        // [b28d-lane15 判词·N] 81=81；残差 = ①ORIG `lea -0x1(%eax),%edx` vs 4.4
        // `sub $0x1,%edx`（N1 常量算术形）②窄类型 and 宽度 + 写回求值序（同上），封顶。
        *reinterpret_cast<int*>(&m_trigger0) =
            *reinterpret_cast<int*>(&m_trigger0) - 1;
        return;
    }
    if (v & 0x10)
    {
        if (((unsigned short)m_trigger0 & 0x1ff) > 0)
            m_trigger0 = (short)(((unsigned short)m_trigger0 & 0xfe00) |
                                 ((((unsigned short)m_trigger0 & 0x1ff) + 0x1ff) & 0x1ff));
    }
    if (v & 0x20)
    {
        if (((unsigned short)m_trigger1 & 0x1ff) > 0)
            m_trigger1 = (short)(((unsigned short)m_trigger1 & 0xfe00) |
                                 ((((unsigned short)m_trigger1 & 0x1ff) + 0x1ff) & 0x1ff));
    }
    if (v & 0x40)
    {
        if (((unsigned short)m_trigger2 & 0x1ff) > 0)
            m_trigger2 = (short)(((unsigned short)m_trigger2 & 0xfe00) |
                                 ((((unsigned short)m_trigger2 & 0x1ff) + 0x1ff) & 0x1ff));
    }
}

// [b28d-lane15 判词·N（跨版本窄类型位运算 lowering）] ORIG 0x0836dd68 每字段
// `movzwl (%eax),%eax; and $0x1ff,%ax; movzwl %ax,%eax`（16 位 and 后零扩展）；
// 4.4 无槽形恒 `movzwl; movzwl %ax,%eax; and $0x1ff,%eax`（32 位 and；探针
// /tmp/b28d_l15_probe2.cpp k/n）。要出 16 位 and 须具名 unsigned short 局部
// （探针 m），但会新增 ushort 槽（ORIG 仅一个 int 槽 -0x4）⇒ 无槽形不可复现。
int stQuestTriggerState_t::getTrigger()
{
    int r = (int)((unsigned short)m_trigger0 & 0x1ff);
    r |= (((int)((unsigned short)m_trigger1 & 0x1ff)) << 9);
    r |= (((int)((unsigned short)m_trigger2 & 0x1ff)) << 18);
    return r;
}
