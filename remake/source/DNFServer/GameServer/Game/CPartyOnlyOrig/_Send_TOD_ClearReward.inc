int CParty::_Send_TOD_ClearReward()
{
    PacketGuard packet;
    packet.put_header(0, 0x15b);
    // b28d-solo-CPartyOnlyOrig：具名成员（ORIG +0x32c disp 直读；裸
    // `*(int*)&m_battleData` 产 `add $0x32c; mov (%eax)` 两条）。ORIG 另有
    // `mov 0x330(%eax),%edx` 冗余装载（4.1 产物，N18a 封顶）。
    packet.put_int(m_battleData.m_field0);   // ORIG *(int*)(this+0x32c)
    // b28d-solo-CPartyOnlyOrig：ORIG +0x67 `mov %eax,-0x14` 指针落槽（P0 槽
    // 往返 ⇒ 原码有具名指针局部，此前留 edx 无槽）⇒ 补具名 userState；
    // +0x77 getLastEnterLayer 用 sret 匿名临时 -0x16 后直接 GetLayer
    //（P278 匿名 sret 全表达式链）。ORIG 无条件（无 null 检查）。
    TOD_UserState* userState = reinterpret_cast<TOD_UserState*>(
        getManager()->GetCharacExpandData(
            static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xd)));
    packet.put_short(static_cast<unsigned short>(
        userState->getLastEnterLayer().GetLayer()));
    packet.put_byte(10);   // ORIG +0x09c 固定 byte 10
    // [P171/P184 死块照抄] ORIG +0x0af..+0x0bf：一次 size() 调用 + `cmp $0x9,%eax`
    // （无分支消费者）。探针 /tmp/probe_hnq/t.cpp h8 实证：空 if 体被 4.4 前端删除
    // 但比较与调用保留，逐条同形（`> 9` 无符号比较）。
    if (reinterpret_cast<CBattle_Field*>(&m_battleData)
            ->m_todReward.m_todMap.size() > 9)
    {
    }
    // P48/P157：size()/operator[] 每使用点独立求值（ORIG 循环条件每轮
    // reload this + call size，无缓存）。
    for (unsigned int i = 0;
         i < reinterpret_cast<CBattle_Field*>(&m_battleField)
                 ->m_todReward.m_todMap.size();
         ++i)
    {
        packet.put_int(
            reinterpret_cast<CBattle_Field*>(&m_battleField)
                ->m_todReward.m_todMap[i].first);
        packet.put_int(
            reinterpret_cast<CBattle_Field*>(&m_battleField)
                ->m_todReward.m_todMap[i].second);
    }
    for (unsigned int i = 0;
         i < 9U -
            reinterpret_cast<CBattle_Field*>(&m_battleField)
                ->m_todReward.m_todMap.size();
         ++i)
    {
        packet.put_int(-1);
        packet.put_int(0);
    }
    packet.put_int(reinterpret_cast<CBattle_Field*>(&m_battleField)
                       ->m_todReward.m_todPos.first);
    packet.put_int(reinterpret_cast<CBattle_Field*>(&m_battleField)
                       ->m_todReward.m_todPos.second);
    packet.finalize(true);
    send_to_party(packet);
    return 1;
