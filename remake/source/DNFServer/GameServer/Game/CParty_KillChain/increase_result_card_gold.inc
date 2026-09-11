// ============================================================================
// increase_result_card_gold —— ORIG T 0x0859ba98。
// 遍历 result.m_dropItems 与成员槽双游标：checkValidUser(i) 且迭代器未尽时，
// 以 RewardCardGold(type=0xb) 结算该成员金卡加成并回写 item.m_addInfo2(+0x07)；
// 每次消费一个结果项（迭代器独立于成员循环推进）。
// ============================================================================
// [b28d-solo-CParty_KillChain AE] ORIG 0x0859ba98：77/77 助记符/槽位逐条全等（本波
// fresh .o 复验，routed 口径 identical/ae），vector begin/end/operator* 链与 ORIG 一致，
// P167 CLEAR。
void CParty::increase_result_card_gold(WongWork::stGenerateResult_t& result)
{
    std::vector<Inven_Item>::iterator it = result.m_dropItems.begin();
    for (int i = 0; i <= 3; ++i)
    {
        if (!checkValidUser(i))
            continue;
        // P220 正极性包裹：ORIG `call ==; test; je`（==真跳主流程后）
        if (it != result.m_dropItems.end())
        {
            QuickParty::RewardData_Param param(this);
            param.m_param4 = kcbf(this).get_dungeon_index();
            QuickParty::RewardData_Result res;
            // P167 判据：ORIG 0x0859bb46/0x0859bb7f 调 `operator*`(deEv) 而非
            // `operator->`(ptEv) ⇒ 源形必须写 (*it).m（it-> 会多 2 个 ptEv 调用点）。
            res.m_value00 = (*it).m_addInfo2;   // item+0x07 金卡金额
            set_basic_reward(QuickParty::BASIC_REWARD_CARD_GOLD, param, res);
            (*it).m_addInfo2 = res.m_value00;
            ++it;
        }
    }
}