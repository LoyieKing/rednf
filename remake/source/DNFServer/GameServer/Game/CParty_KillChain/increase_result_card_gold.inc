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
