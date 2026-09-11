// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08214b96 orig/ours=165/176 ndiff-blocks=27
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    -0x29(%ebp),%eax | movl   $&_ZZN30Dispatcher_AuctionRegistCancel12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movzbl -0x2
int Dispatcher_AuctionRegistCancel::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08214b96..0x08214e43 (0x2ae 字节)。GM 表 _ZL6gmList @0x093abb4c。
    // 包：PCK_AUCTION_REGIST_CANCEL_GP（ctor W 0x0822ea1c）/ _GA（ctor W 0x0822e5c4）
    //   uid@0x12, characNo@0x16, auctionId[8]@0x1a。
    // [R4BigTUDispatchers 2026-09-11 · 符号名轴] 删函数内定义 ⇒ 用 TU 文件级 `_ZL6gmList`。

    // ORIG 0x08214b96：[4] `lea -0x29(%ebp),%eax` 直取址、无 `movb $0x0` 预清（P328⑤）。
    unsigned char payType;
    if (packet.get_byte(payType) != 1)
        return LineFunc(0xc47f, __PRETTY_FUNCTION__, 0, 0);
    if (payType > 1)
        return LineFunc(0xc482, __PRETTY_FUNCTION__, 0, 0);

    const int pay = payType;
    const int preCheck = user->AuctionPreCheck((ENUM_CMDPACKET)0xbb, (PAY_TYPE)pay);
    if (preCheck >= 0)
        return preCheck;

    const unsigned int secuErr =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x33);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbb, (unsigned char)(secuErr & 0xff));
        return 0;
    }

    if (pay == 1)
    {
        PCK_AUCTION_REGIST_CANCEL_GP pck;
        // [G5DsigE] ORIG 0x08214cxx 直证 pck 基址 -0x4b：uid 落 -0x39=pck+0x12、
        //   characNo 落 -0x35=pck+0x16（**无 body 指针槽、无 uid/characNo 局部**）。
        //   m_data 起点 = PACKET_HEADER 0x12 ⇒ +0/+4/+8 即 +0x12/+0x16/+0x1a。
        *reinterpret_cast<unsigned int*>(pck.m_data + 0x0) = user->GetUID();
        *reinterpret_cast<int*>(pck.m_data + 0x4) = user->getCurCharacNo();
        if (packet.get_binary(pck.m_data + 0x8, 8) != 1)
            return LineFunc(0xc4b1, __PRETTY_FUNCTION__, 0, 0);

        GlobalData::s_cera_auction_proxy->SendPacket(
            reinterpret_cast<nsl::PACKET_HEADER*>(&pck));
    }
    else
    {
        PCK_AUCTION_REGIST_CANCEL_GA pck;
        // [G5DsigE] 同 GP 支：直写包体成员（ORIG 复用 -0x39/-0x35 disp store）。
        *reinterpret_cast<unsigned int*>(pck.m_data + 0x0) = user->GetUID();
        *reinterpret_cast<int*>(pck.m_data + 0x4) = user->getCurCharacNo();
        if (packet.get_binary(pck.m_data + 0x8, 8) != 1)
            return LineFunc(0xc4c4, __PRETTY_FUNCTION__, 0, 0);

        GlobalData::s_auction_proxy->SendPacket(
            reinterpret_cast<nsl::PACKET_HEADER*>(&pck));
    }

    for (int i = 0; i <= 4; i++)
    {
        if (gmList[i] == user->getCurCharacNo())
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0xc4d4, 0);
            trace("Trace Auction Delay, %s(%d)", "REQ AuctionRegistCancel",
                  user->getCurCharacNo());
            break;
        }
    }
    return 0;
}