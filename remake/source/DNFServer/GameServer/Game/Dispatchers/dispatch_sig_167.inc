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
    // [G1-Dispatchers] 块物理序按 ORIG：`je`（result==0）跳过失败块 ⇒ 源码为
    // `if (result != 0) {失败} else {成功}`（ORIG 直落块含 put_byte(0)/put_byte(result)/
    // finalize/Send 四 call；我方旧形反序，diff 首差即 `je` vs `jne`）。
    if (result != 0)
    {
        guard.put_byte(0);
        guard.put_byte(result);
        guard.finalize(true);
        user->Send(guard);
    }
    else
    {
        guard.put_byte(1);
        guard.put_byte((int)(char)step);
        guard.put_byte((int)useQuickPartyAdd);
        guard.put_byte((int)rewardMap.size());
        // P96：ORIG 侧为非 const `_Rb_tree_iterator`（0x082225.. 段 `_ZNKSt17_Rb_tree_iterator…ptEv/ne/ppEv`），
        // 我方 const_iterator 会额外生成转换构造 call 与 const 版 ptEv/ne（p167 EXTRA 5 项）。
        for (std::map<int, int>::iterator it = rewardMap.begin(); it != rewardMap.end(); ++it)
        {
            guard.put_int(it->first);
            guard.put_int(it->second);
        }
        guard.finalize(true);
        user->Send(guard);
    }
    return 0;
