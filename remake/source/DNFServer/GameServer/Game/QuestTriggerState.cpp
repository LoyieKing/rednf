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
// [E04 2026-09-10 解 N：位域建模] 旧判词「跨版本窄类型 lowering 不可复现」作废。
//   真因 = 原码字段是 16 位容器 9 位位域（QuestTriggerState.h 位域 + 同址 int 视图建模）：
//   位域读 = `movzwl; andw $0x1ff,%ax; movzwl %ax,%eax`（16 位 and 后零扩展）；
//   位域写回 = 保留同容器高 7 位（`and $0xfe00,%ax`）；operator= 解包 = `and $0x3fe00;
//   sar $0x9` / `and $0x7fc0000; sar $0x12` ⇒ 原码 (v & (0x1ff<<N)) >> N 形。
//   双编译器探针 /tmp/qts/b.cpp（4.4.7）四函数与 ORIG 逐条一致；4.1.2 同形
//   （/tmp/qts/p2_41.s m8 位域读），非跨版本残差。

// [§4.4 合并 2026-09-10] 类型唯一定义点 = QuestTriggerState.h（本 TU 曾重复定义
// class 镜像，与 Quest.h 的 struct 逐 token 不同：方法声明序异、无 setTrigger ⇒ ODR
// 违规）。成员布局（4×short / 8 字节）与原权威定义逐字段一致，合并零 ABI 变化。
#include "QuestTriggerState.h"

stQuestTriggerState_t::stQuestTriggerState_t()
{
    // ORIG：movl $0x0,(%eax); movl $0x0,0x4(%eax) —— 按 8 字节（2×int）整块清零
    *(long long*)this = 0;
}

void stQuestTriggerState_t::operator=(int const& v)
{
    // ORIG 0x086ad56c：清 8 字节 → 三个位域各自「值截断 → 读容器保留高 7 位 → or → 写回」。
    // [E04 2026-09-10] 位域版逐条复现（探针 /tmp/qts/b.cpp asA，45=45 identical）：
    //   字段1 三次 `and $0x1ff,%ax`（(short) cast + 位域赋值截断）；
    //   字段2/3 `and $0x3fe00,%eax; sar $0x9` / `and $0x7fc0000,%eax; sar $0x12`
    //   ⇒ 原码是 (v & (0x1ff<<N)) >> N 形（4.4 不做 4.1 的「先移位后掩码」折叠）。
    //   返回类型 void：ORIG 尾部 `pop %ebp; ret` 无 eax 装载（返回类型不入 mangled）。
    *(long long*)this = 0;
    m_trigger0 = (short)(v & 0x1ff);
    m_trigger1 = (short)((v & 0x3fe00) >> 9);
    m_trigger2 = (short)((v & 0x7fc0000) >> 18);
}

void stQuestTriggerState_t::incTirggerState(char v)
{
    // ORIG 0x086ad602：读 `movzwl; andw $0x1ff,%ax; movzwl %ax,%eax; cmp $0x3fe`，
    // 自增段 `movzwl; andw $0x1ff,%ax; add $0x1,%eax; andw $0x1ff,%ax` + 位域写回
    // （保留高 7 位）。位域 `++m_triggerN` 逐条复现（探针 /tmp/qts/b.cpp，73=73）。
    if (v & 0x10)
    {
        if (m_trigger0 <= 0x3fe)
            ++m_trigger0;
    }
    if (v & 0x20)
    {
        if (m_trigger1 <= 0x3fe)
            ++m_trigger1;
    }
    if (v & 0x40)
    {
        if (m_trigger2 <= 0x3fe)
            ++m_trigger2;
    }
}

void stQuestTriggerState_t::subTriggerState(char v)
{
    if (v == 0)
    {
        // ORIG 0x086ad6fc：`mov (%eax),%eax; lea -1(%eax),%edx; mov %edx,(%eax)` ——
        // 对 +0x00 的 4 字节视图（m_trigger0/m_trigger1 同容器）裸自减 1（m_low32）。
        --m_low32;
        return;
    }
    // ORIG 自减段：读同 inc（`add $0x1ff,%ax` 即 16 位 -1）+ 位域写回保留高 7 位。
    if (v & 0x10)
    {
        if (m_trigger0 > 0)
            --m_trigger0;
    }
    if (v & 0x20)
    {
        if (m_trigger1 > 0)
            --m_trigger1;
    }
    if (v & 0x40)
    {
        if (m_trigger2 > 0)
            --m_trigger2;
    }
}

// ORIG 0x0836dd68：每字段 `movzwl; andw $0x1ff,%ax; movzwl %ax,%eax`（位域读），
// 后两字段 `shl` 后 `or %eax,-0x4(%ebp)` ⇒ 具名 int r + |= 形（探针 23=23）。
int stQuestTriggerState_t::getTrigger()
{
    int r = m_trigger0;
    r |= (m_trigger1 << 9);
    r |= (m_trigger2 << 18);
    return r;
}
