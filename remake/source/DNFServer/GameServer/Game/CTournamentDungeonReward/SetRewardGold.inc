void CTournamentDungeonReward::SetRewardGold(
    WongWork::stGenerateResult_t& result, float goldRate)
{
    for (int card = 0; card < 2; ++card)
    {
        for (int slotIdx = 0; slotIdx < 2; ++slotIdx)
        {
            if (m_rewards[card][slotIdx].m_id == 0)
            {
                // ORIG：具名 iterator 局部（槽 -0x18）+ end() 临时（-0x14）
                // + operator!=/operator-> 实例；4.4 的 empty() 是 out-of-line
                // call 且带 bool 物化岛（P197④ 反向），故写 begin()!=end() 形。
                std::vector<Inven_Item>::iterator it =
                    result.m_dropItems.begin();
                if (it != result.m_dropItems.end())
                {
                    // ORIG 直读 +0x07（m_addInfo2），无 get_add_info call；
                    // x87 RC=truncate 直接截断（fildl+fmuls+fistpl，无 +0.5）
                    m_rewards[card][slotIdx].m_field04 =
                        (int)((float)it->m_addInfo2 * goldRate);
                }
            }
        }
    }
