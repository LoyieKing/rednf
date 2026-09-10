int Dispatcher_RequestConditionEventReward::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x082223a6..0x082226bf (0x31a 字节)。

    if (user == 0)
        return LineFunc(0xdbda, __PRETTY_FUNCTION__, 0, 0);
    if (user->get_state() != 3)
        return 0;

    short step = 0;       // -0x16（get_short(short&)）
    char useQuickPartyAdd = 0;   // -0x17（get_byte(char&)）
    if (packet.get_short(step) != 1)
        return LineFunc(0xdbe1, __PRETTY_FUNCTION__, 0, 0);
    if (packet.get_byte(useQuickPartyAdd) != 1)
        return LineFunc(0xdbe2, __PRETTY_FUNCTION__, 0, 0);

    int result = 0;   // -0x10
    CConditionEventManager* mgr = G_CGameManager()->GetConditionEventManager();
    std::map<int, int> rewardMap;   // -0x30
    if (mgr != 0)
        result = mgr->RequestConditionEventReward(user, step, useQuickPartyAdd, rewardMap);

    PacketGuard guard;
    guard.put_header(1, 0x107);
    if (result == 0)
    {
        guard.put_byte(1);
        guard.put_byte((int)(char)step);
        guard.put_byte((int)useQuickPartyAdd);
        guard.put_byte((int)rewardMap.size());
        for (std::map<int, int>::const_iterator it = rewardMap.begin(); it != rewardMap.end(); ++it)
        {
            guard.put_int(it->first);
            guard.put_int(it->second);
        }
        guard.finalize(true);
        user->Send(guard);
    }
    else
    {
        guard.put_byte(0);
        guard.put_byte(result);
        guard.finalize(true);
        user->Send(guard);
    }
    return 0;
