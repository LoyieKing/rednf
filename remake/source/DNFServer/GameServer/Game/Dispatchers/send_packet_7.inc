void Dispatcher_GetAchievementReward::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x8140daa：sendAchievementReward(user, res+0x08, &items) 回写 res+0x04；
    // error==0 且 items 非空时追加 items 逐项 _RewardItem(0xc)，否则 put_int(count)。
    CMDPacketStruct::_ResAdvanceAltarGetAchievementReward& res =
        *reinterpret_cast<CMDPacketStruct::_ResAdvanceAltarGetAchievementReward*>(&resp);
    PacketGuard guard;
    guard.put_header(1, 0x243);
    guard.put_byte(1);
    int count = 0;                                  // ORIG -0xc(%ebp) 初始 0
    const std::vector<_RewardItem>* items = 0;      // ORIG -0x28(%ebp)
    res.m_nErrorCode =
        user->m_advanceAltar.sendAchievementReward(user, res.m_nAchievementIdx, &items);
    guard.put_binary((char*)&res, 0xc);
    if (res.m_nErrorCode == 0 && items != 0)
    {
        count = static_cast<int>(items->size());
        guard.put_int(count);
        for (std::vector<_RewardItem>::const_iterator it = items->begin();
             it != items->end(); it++)   // ORIG 后置 ++：operator++(tmp,&it,0) 三参形态
        {
            guard.put_binary((char*)&*it, 0xc);
        }
    }
    else
    {
        guard.put_int(count);
    }
    guard.finalize(true);
    user->Send(guard);
