int CMission_combo_clear::_Reward(CUser& user) const
{
    combo_script_key key;
    key.m_job = (char)user.get_charac_job();
    key.m_growType = (short)user.getCurCharFirstGrowType();

    std::map<combo_script_key, combo_script_data>::const_iterator it =
        m_comboMap.find(key);
    // b28d-50：ORIG 0x085e43xx 实参序 (%esp)=end、0x4(%esp)=it（同 _isAcceptCondition，
    // P74补②/P220 反序）。
    // [2026-09-10 P292 正极性包裹] ORIG 0x085e41af `test %al,%al; je 085e42e1`
    //   —— je 落**函数尾部共享块** `mov $0x1,%eax`（+0x1ad），cond 真走 body 直落
    //   ⇒ 源码是 `if (it != end) { body } return 1;`；早返回形 `if (end == it)
    //   return 1;` 会就地物化 `mov $0x1,%ebx; jmp`（我方 127 vs ORIG 125 两段差）。
    if (it != m_comboMap.end())
    {
        std::vector<std::pair<int, int> > items;
        items.push_back(it->second.m_bonus);

        std::vector<int> outSlots;
        // b28d-50：ORIG 预清 `movl $0x0,-0xc(%ebp)` 在 call 前（P198①两句式）。
        int ret = 0;
        ret = ((CInventory*)user.getCurCharacInvenW())
                  ->insert_event_items(items, outSlots,
                                       (eMoneyAddReason)0x1e,
                                       (eItemAddReason)0x33, true,
                                       "game_server_msg_173",
                                       "game_server_msg_174");
        if ((ret > 0) && (outSlots.size() > 2))
        {
            user.SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_0,
                                outSlots[0]);
        }
    }
    return 1;
