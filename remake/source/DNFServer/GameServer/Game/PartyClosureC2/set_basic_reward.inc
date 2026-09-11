// ===========================================================================
// CParty::set_basic_reward —— T 0x0859b992
// 报告：set_basic_reward.md。仅快速队伍生效：
//   1) param.m_param4 已在奖励管理器副本列表中 → 忽略；
//   2) get_reward_type_object(BASIC, type) 命中 → vtable 槽1 process(param,result)。
// ===========================================================================
void CParty::set_basic_reward(QuickParty::BasicRewardType type,
                              QuickParty::RewardData_Param param,
                              QuickParty::RewardData_Result& result)
{
    // [b28d-solo-PartyClosureC2] ORIG 0x0859b992：p167 实证 ORIG 调
    // `vector<int>::begin() const` / `__normal_iterator<int const*,...>` ⇒
    // 先例形 `((const std::vector<int>&)...).begin()`（CBattle_Field.cpp:863 /
    // QuickParty.cpp:165）：const 重载直返 const_iterator，无转换构造 call，
    // sret 直接落 -0x10/-0x14 两槽（帧 0x24）。
    if (!is_quick_party())                      // xor $1 后 test：非快速队直接返回
        return;

    std::vector<int>::const_iterator it =
        ((const std::vector<int>&)
            G_CGameManager()->GetQuickPartyRewardManager()->get_dungeon_idx_list())
            .begin();
    std::vector<int>::const_iterator end =
        ((const std::vector<int>&)
            G_CGameManager()->GetQuickPartyRewardManager()->get_dungeon_idx_list())
            .end();
    for (; it != end; ++it)
    {
        if (param.m_param4 == *it)              // cmp 0x14(%ebp)（Param 第二字）
            return;
    }

    QuickParty::IRewardType* reward =
        G_CGameManager()->GetQuickPartyRewardManager()->get_reward_type_object(
            QuickParty::ENUM_REWARD_TYPE_BASIC, (unsigned int)type);
    if (reward != 0)
        reward->process(param, result);         // vtable 槽1
}