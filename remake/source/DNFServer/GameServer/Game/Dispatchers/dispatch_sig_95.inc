int Dispatcher_BiddingRoutingItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820xxxx：`call; cmp $0x5,%eax; je; cmp $0xc,%eax; je` —— 一次调用、
    // 连续两次用 %eax 直接比较（免槽）。我方具名 int 局部使 4.4 spill 成
    // `mov %eax,-0x14(%ebp); cmpl $5,-0x14(%ebp)` 两处（技巧28：register 钉 %eax 破 spill）。
    // b28d-solo-Dispatchers verdict=DIFF [N-P112] ORIG 0x082255ca orig/ours=116/114
    //   ORIG 无槽 cmp 链（`call; cmp $5,%eax; je; cmp $0xc,%eax; je`）；switch(call())
    //   与具名 int 局部两形实测均被 4.4 spill（mov %eax,-0x14; cmpl $5,-0x14）⇒ 封顶。
    int state = user->get_state();
    if (state != 5 && state != 0xc)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x12d, 0x13);
        return 0;
    }
    CParty* party = (CParty*)user->GetParty();
    if (party == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x12d, 0x13);
        return 0;
    }
    char selection = 0;                              // -0x11
    if (!packet.get_byte(selection))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x12d, 0x13);
        return 0;
    }
    CSpecialItemRoutingManager* routing = G_CGameManager()->GetSpecialItemRoutingManager();
    // X1 位点（台账 ours_only=sete1,test1）：ORIG 此处是单段 `xor $0x1,%eax;
    // testb %al,%al; je` 岛，而 4.4 对 `!intcallee()` 恒折成 `test;sete %al;test`
    // 两段 ⇒ 用「绑 %eax 具名 unsigned char + ^= 1」形复现（判据见 X 族五级序）。
    unsigned char arriveBad =
        (unsigned char)routing->ArriveMemberSelection(user, party, selection);
    arriveBad ^= 1;
    if (arriveBad)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x12d, 0x13);
        return 0;
    }
    PacketGuard packet_;                             // -0x20
    packet_.put_header(1, 0x12d);
    packet_.put_byte(1);
    packet_.finalize(true);
    user->Send(packet_);
    return 0;
