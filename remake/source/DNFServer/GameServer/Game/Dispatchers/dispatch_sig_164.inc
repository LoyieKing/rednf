// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08216966 orig/ours=141/147 ndiff-blocks=20
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    -0x21(%ebp),%eax | movl   $&_ZZN36Dispatcher_AuctionMyRegistedItemInfo12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movzb
int Dispatcher_AuctionMyRegistedItemInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // [L7DispG1 2026-09-11] ORIG 0x08216966 首指令区 `lea -0x21(%ebp),%eax`（无 `movb $0x0`）
    //   ⇒ 原码未预清（P328⑤：get_byte 失败即 return，成功路径必已赋值）；ours 多 1 条 `movb $0x0`。
    unsigned char channel;                                 // -0x21
    if (!packet.get_byte(channel))
    {
        return LineFunc(0xc78e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (channel > 1)
    {
        return LineFunc(0xc791,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // [L7DispG1 2026-09-11 负结论] 试按 ORIG（`movzbl -0x21; movzbl %al,%eax; mov %eax,-0x10`）
    //   把 channel 提升为具名 int `channelNo` 并替换后续 3 个用点 ⇒ 读数**零变化**（147→147，
    //   4.4 的局部常量传播把该槽消掉）⇒ 回滚保最小 diff。
    if (IsAuctionServerDisconnected(channel))              // 0x082138f2
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbf, 0x8c, (PAY_TYPE)(unsigned int)channel);
        return 0;
    }
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xc7b6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (channel == 1)
    {
        PCK_AUCTION_MY_REGISTED_ITEM_INFO_GP pkt;          // -0x3b ctor 0x0822eb70
        pkt.m_uid = user->GetUID();
        pkt.m_characNo = (unsigned int)user->getCurCharacNo();
        GlobalData::s_cera_auction_proxy->SendPacket((nsl::PACKET_HEADER*)&pkt);
        return 0;
    }
    PCK_AUCTION_MY_REGISTED_ITEM_INFO_GA pkt;              // -0x55 ctor 0x0822e7ac
    pkt.m_uid = user->GetUID();
    pkt.m_characNo = (unsigned int)user->getCurCharacNo();
    GlobalData::s_auction_proxy->SendPacket((nsl::PACKET_HEADER*)&pkt);
    for (int i = 0; i < 5; i++)                            // gmList 0x093701d8
    {
        if (gmList[i] == (int)user->getCurCharacNo())
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xc823, 0);
            trace("Trace Auction Delay, %s(%d)", "REQ AuctionMyRegistedItemInfo",
                  (int)user->getCurCharacNo());
            break;
        }
    }
    return 0;
}