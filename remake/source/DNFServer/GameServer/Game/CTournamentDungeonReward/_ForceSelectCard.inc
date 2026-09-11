// ============================================================================
// ORIG 0x082853b8（报告 _ForceSelectCard.md）：type>1 直接返回；
// 对两槽：m_flag08==0xff（未选）时遍历 members，凡 !isMemberSelectCard(type,*it)
// 者写入 m_flag08（低字节）。持续遍历不提前退出（ORIG 形态）。
// [b28d-N82] fresh 复验 95/96：残差 = 多 push %edi + 帧 0x20→0x2c + 循环槽平移。
// [b28d-solo-CTournamentDungeonReward] DIFF 95/101：修 `++it`→`it++`（P28 增量侧：
// ORIG 增量段 `operator++(it,0)` + 独立 sret 临时 -0x10）；修后结构/槽序与 ORIG
// 全等，残 = N18a×1 + push %edi/N115 帧 0x10；p167 7=7 CLEAR。
// [SweepCTournament 2026-09-09] 复核 96/101：残 = N115 push %edi + N18a 中转×3；
// p167 7=7 CLEAR；jcc（jg/jne/je/setle）与 ORIG 逐位全等 ⇒ N 封顶。
// ============================================================================
void CTournamentDungeonReward::_ForceSelectCard(
    ENUM_TOURNAMENT_REWARD_CARD_TYPE type, const std::vector<int>& members)
{
    if ((int)type > 1)
        return;

    for (int slot = 0; slot < 2; ++slot)
    {
        if (static_cast<unsigned char>(m_rewards[(int)type][slot].m_flag08) == 0xff)
        {
            // ORIG 增量段调 operator++(it,0)（后置，带独立 sret 临时槽 -0x10）⇒
            // 源形必写 it++（P28 增量侧推论）。显式 itEnd 缓存实测 ndiff 63→73
            // 反劣（4.4 不按 P13 缓存 end），故维持 `it != members.end()`。
            for (std::vector<int>::const_iterator it = members.begin();
                 it != members.end(); it++)
            {
                if (!isMemberSelectCard(type, *it))
                    m_rewards[(int)type][slot].m_flag08 = *it;
            }
        }
    }
}