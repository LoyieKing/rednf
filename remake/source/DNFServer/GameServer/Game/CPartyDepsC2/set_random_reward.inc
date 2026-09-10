void CParty::set_random_reward(QuickParty::RandomBuffType type,
                               QuickParty::RewardData_Param param,
                               QuickParty::RewardData_Result& result)
{
    // [b28d-solo-CPartyDepsC2] 修 N18b 变体（ORIG 0x0859b8fa）：`m_field1ae8`
    // （unsigned int）与 enum 形参直接比较时 4.4 恒两段寄存器装载（P209 实测：
    // 字段/形参异型），ORIG 0x0859b925 段为同型 mem 直比
    // `mov 0x1ae8(%eax),%eax; cmp 0xc(%ebp),%eax`。用 enum 同型引用视图读
    // （P235 同址双视角，探针 /tmp/b28d_c2_probe/p6.cpp n0/n1 逐条全等）⇒ 字段本体
    // 仍为 unsigned int（store 段 0x0859b814 P211 异型 RHS-first 不变）。
    if (!is_quick_party())                  // is_quick_party W 0x0822d952
        return;
    if (m_field1ae8 == 0xb)                 // BASIC_REWARD_CARD_GOLD 占用时跳过
        return;
    if (m_field1ae8Type != type)            // 同址 enum 视角（CParty.h union）
        return;

    QuickParty::IRewardType* obj =
        G_CGameManager()->GetQuickPartyRewardManager()->get_reward_type_object(
            (QuickParty::ENUM_REWARD_TYPE)1, (unsigned int)type);
    // ORIG 0x0859b95a `je 0x0859b98c` 直落主出口（无 nop 岛）⇒ 正向包裹（P257/P285：
    // 早返形恒多一岛）。vtable 槽1（vptr+0x04）。
    if (obj != 0)
        obj->process(param, result);
