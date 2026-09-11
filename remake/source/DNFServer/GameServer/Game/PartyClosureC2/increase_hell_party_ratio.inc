// ===========================================================================
// CParty::increase_hell_party_ratio —— T 0x0859bba8
// 报告：increase_hell_party_ratio.md。平均等级 >59 时走 HellPartyDifficulty
// 基础奖励：str[1]/str[2] 作为 RewardData_Result 写入槽进出 process。
// ===========================================================================
void CParty::increase_hell_party_ratio(char* str)
{
    if (!(getPartyMemberAvgLevel() > 0x3b))     // setle+jne：<=59 直接返回
        return;

    QuickParty::RewardData_Param param(this);   // C1 W 0x08469976（头内联同型）
    // ORIG：ctor 置 m_param4=0 后，-0xc(%ebp) 立即被 get_dungeon_index 覆写，
    // 即 Param 第二字 = 当前副本 idx（供 set_basic_reward 副本去重）。
    param.m_param4 = bf(this).get_dungeon_index();
    QuickParty::RewardData_Result result;       // clear 后 +0x14=1.0f/+0x18=1
    result.m_value00 = str[1];                  // movsbl str[1]
    result.m_value04 = str[2];                  // movsbl str[2]

    set_basic_reward(QuickParty::BASIC_REWARD_HELL_DIFFICULTY /*0xc*/, param,
                     result);

    str[1] = static_cast<char>(result.m_value00);
    str[2] = static_cast<char>(result.m_value04);
}