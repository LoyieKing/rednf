int Dispatcher_AuctionMyBiddingInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08216b9a
    // ORIG _ZL6gmList：TU 内 static int[5]（0x093701d8，0x14 字节 .data，初值忠实转写）
    static int s_gmList[5] = {0x4c2dc8, 0x5cf723, 0xa9c742, 0xa2ad85, 0x9ae1ae};
    int result;

    // call CAuctionServerProxy::IsRunning(s_auction_proxy); xor $1 → !IsRunning
    if (!GlobalData::s_auction_proxy->IsRunning())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc0, 0x8c);
        return 0;
    }

    // cmp $0x2,get_state; jle → state<=2 即 invalid；否则 getCurCharacR==0 亦 invalid
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xc836,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    // PCK_AUCTION_MY_BIDDING_INFO_GA local_3a（ORIG 栈对象，字段 +0x0e/+0x12 为裸偏移）
    PCK_AUCTION_MY_BIDDING_INFO_GA req;
    *(unsigned int*)((char*)&req + 0x0e) = user->GetUID();               // local_28
    *(unsigned int*)((char*)&req + 0x12) =
        (unsigned int)user->getCurCharacNo();                            // local_24
    GlobalData::s_auction_proxy->SendPacket((nsl::PACKET_HEADER*)&req);

    // ORIG cmpl $0x4,-0xc; setle → i <= 4（含 4，共 5 轮）
    for (int i = 0; i <= 4; ++i)
    {
        if (s_gmList[i] == (int)user->getCurCharacNo())
        {
            int characNo = (int)user->getCurCharacNo();
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xc84e, 0);
            trace("Trace Auction Delay, %s(%d)", "REQ AuctionMyBiddingInfo", characNo);
            break;
        }
    }
    result = 0;
    return result;
