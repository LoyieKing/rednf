// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x082255ca orig/ours=116/114 ndiff-blocks=9
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: cmp    $0x5,%eax | mov    $0x0,%eax | nop
int Dispatcher_BiddingRoutingItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820xxxx：`call; cmp $0x5,%eax; je; cmp $0xc,%eax; je` —— 一次调用、
    // 连续两次用 %eax 直接比较（免槽）。我方具名 int 局部使 4.4 spill 成
    // `mov %eax,-0x14(%ebp); cmpl $5,-0x14(%ebp)` 两处（技巧28：register 钉 %eax 破 spill）。
    // b28d-solo-Dispatchers verdict=DIFF [N-P112] ORIG 0x082255ca orig/ours=116/114
    //   ORIG 无槽 cmp 链（`call; cmp $5,%eax; je; cmp $0xc,%eax; je`）；switch(call())
    //   与具名 int 局部两形实测均被 4.4 spill（mov %eax,-0x14; cmpl $5,-0x14）⇒ 封顶。
    // [R16a13 · P251] ORIG 0x082255dd..0x082255e5 = 无槽 cmp 链
    //   `call get_state; cmp $0x5,%eax; je +0x42; cmp $0xc,%eax; je +0x45`
    //   且两 case 体均为空（+0x42 `nop;jmp`、+0x45 `nop`，直落共享成功块）
    //   ⇒ 原码是 `switch (get_state())`（case 5/case 0xc 空体 + default 错误），
    //   不是 `if (v != 5 && v != 0xc)` + 具名 int 局部（后者使 4.4 spill 成
    //   `mov %eax,-0x18(%ebp); cmpl $0x5,-0x18(%ebp)`）。
    switch (user->get_state())
    {
    case 5:
    case 0xc:
        break;

    default:
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
    // [R16a13 · P292/P168a①] ORIG 0x08256ba 形 = `call; xor $0x1,%eax; test %al,%al;
    // je`（callee 返回 bool → 裸形直产；4.4 对 int callee 的任何调用点写法恒产
    // `test;sete` 两段且具名 uchar `^= 1` 形落栈槽物化 +1 条并顶浅后续槽）。
    // 原 X1 形（unsigned char arriveBad + ^= 1）已按 P168a④ 定论废弃：
    // ArriveMemberSelection 返回类型已改 bool（见 CSpecialItemRoutingManager.h）。
    if (!routing->ArriveMemberSelection(user, party, selection))
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
}