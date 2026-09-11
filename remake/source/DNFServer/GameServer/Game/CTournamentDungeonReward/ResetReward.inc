// ORIG 0x8284820：对 2x2 个 TournamentRewardType（+0x8 起）逐个 Clear，
// 最后清零 +0x04。构造函数中的 memset 占位同步移除（成员自动初始化）。
// [b28d-N18a] fresh 复验 36/37：Clear 外联化后帧 0x14 / 槽 -0x8/-0x4 已与 ORIG
// 全等；残 1 行 = 二维索引中间寄存器（ORIG `lea 0x0(,%eax,4),%ecx` 直落 vs
// 4.4 `mov %eax,%edx` 中转，P30/N18a 封顶）。
// [b28d-solo-CTournamentDungeonReward] DIFF 36/37：残 = N18a 二维索引中转×1；
// p167 1=1 CLEAR，语义全等 ⇒ N 封顶。
// [SweepCTournament 2026-09-09] 复核 36/37：探针 12 形（/tmp/probe_r{,2,3}.cpp）
// + 旗标矩阵（i486/regmove/gcse/tune/sched）+ 4.4.6 后端全部同形 ⇒ N 封顶。
// [round5-A17 2026-09-10] 本 TU 10 件 DIFF 全量复核（fresh routed .o + p167 逐件）：
// 残差 100% 为同一族——`m_rewards[2][2]`（元素 0xc、行 0x18）二维索引的**中间结果
// 寄存器角色**（ORIG 4.1：外层索引→%edx、内层→%ecx、`lea 0x0(,%eax,4),%ecx` 直落；
// 4.4.7：外层→%ecx、内层→%eax 并多 1 条 `mov %eax,%edx`），计数 = 每处 +1 条
// （ResetReward/isMemberSelectCard/MakeSelectPacket ×1、SelectCard/_MakeRewardPacket/
// isCardSelectComplete/SetRewardGold ×2、DecisionRewardType ×3、GetItemFromCard ×6），
// 另 _ForceSelectCard(95/96)/SetRewardGold(91/95) 各含 N115 push %edi+帧差。
// 探针（/tmp/probe_tn/a.cpp fA..fJ + b.cpp fK..fQ：引用形/行指针形/一维视图/
// m_rows[i].v[j] 组件形/register 形/unsigned 形/单 case 形）全部产同一 `mov %eax,%edx`
// 形（register 形额外 push %esi/%ebx 反劣）⇒ 无源形可择。
// p167 逐件 **CALLSITE CLEAR**（10/10）⇒ 语义完整。判 N18a 族封顶，勿再试。
void CTournamentDungeonReward::ResetReward()
{
    for (int c = 0; c < 2; ++c)
        for (int r = 0; r < 2; ++r)
            m_rewards[c][r].Clear();
    m_field04 = 0;
}