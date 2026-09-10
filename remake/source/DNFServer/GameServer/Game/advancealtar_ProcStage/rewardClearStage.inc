void ProcStage::rewardClearStage(std::vector<_RewardItem>& out)
{
    if (m_dungeon == 0)
    {
        return;
    }
    out.clear();

    // b28d-solo-ProcStage：P167 符号族——ORIG 迭代器为 const 族
    //   （`__normal_iteratorIPKN…` deEv×5 + ptEv×2、ZNK begin/end、ne(const,const)、
    //   ppEi），且**无** const_iterator 转换构造 ⇒ items 本身是 const 指针。
    const std::vector<_RewardItem>* items = 0;
    // b28d-solo-ProcStage：ORIG 0x812f12d `cmp $1; je <生存块>`（生存块后置）——
    //   4.4 对 if 形 `==1`/`!=1` 恒产 setcc 物化（各试一形），P286 单 case switch
    //   才出 ORIG 的寄存器比较直跳；default 块在前 = ORIG 普通块 fall-through。
    switch (getStageType())
    {
    default:
        // CDungeon+0x82c = m_clearReward（ClearRewardData），元素 stride 0xc = sizeof(vector<_RewardItem>)，
        //   按 difficulty 索引 m_vec[diff]（rewardClearStage.md `add $0x820`/`add $0xc` 折叠 base）。
        items = &m_dungeon->m_clearReward.m_vec[static_cast<unsigned int>(m_difficulty)];
        break;
    case 1:
        // CDungeon+0x850 = m_survivalReward（SurvivalRewardData，rewardClearStage.md `add $0x850`）。
        items = m_dungeon->m_survivalReward.getRoundRewardData(m_survivalRound);
        break;
    }

    std::vector<_RewardItem> mailNormal;   // local_28
    std::vector<_RewardItem> mailEvent;    // local_34
    if (items != 0)
    {
        int n = 0;   // P171 死计数器照抄（ORIG -0xc）
        // P28——ORIG 增量段 0x812f276 为 ppEi（后置）⇒ 写 it++。
        for (std::vector<_RewardItem>::const_iterator it = items->begin();
             it != items->end(); it++)
        {
            if (isOnEvent())
            {
                out.push_back(*it);
                if (it->m_type == 0)   // P167：ORIG ptEv（非 deEv）
                {
                    mailNormal.push_back(*it);
                }
                else
                {
                    mailEvent.push_back(*it);
                }
            }
            else if (it->m_type == 0)   // P167：ORIG ptEv
            {
                out.push_back(*it);
                mailNormal.push_back(*it);
            }
        }
        (void)n;   // P171：ORIG 仅 `movl $0,-0xc` 初始化、无自增（死写入照抄）
    }
    // b28d-solo-ProcStage：P197④——ORIG 0x812f2c2 `call size; test; setne; test; je`
    //   ⇒ 原码 `size() != 0`（empty() 在 4.4 恒 out-of-line call 且无 setne 段）。
    if (mailNormal.size() != 0)
    {
        sendMailRewardItem(m_characData->getUser(), mailNormal,
                           "chn_game_server_msg_88", "chn_game_server_msg_89");
    }
    if (isOnEvent() && mailEvent.size() != 0)
    {
        sendMailRewardItem(m_characData->getUser(), mailEvent,
                           "chn_game_server_msg_86", "chn_game_server_msg_87");
    }
