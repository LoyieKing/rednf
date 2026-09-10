int Dispatcher_AuctionMyRegistedItemInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    unsigned char channel = 0;                             // -0x21
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
        *(unsigned int*)((char*)&pkt + 0xe) = user->GetUID();        // local_2d
        *(unsigned int*)((char*)&pkt + 0x12) = (unsigned int)user->getCurCharacNo();  // local_29
        GlobalData::s_cera_auction_proxy->SendPacket((nsl::PACKET_HEADER*)&pkt);
        return 0;
    }
    PCK_AUCTION_MY_REGISTED_ITEM_INFO_GA pkt;              // -0x55 ctor 0x0822e7ac
    *(unsigned int*)((char*)&pkt + 0xe) = user->GetUID();            // local_47
    *(unsigned int*)((char*)&pkt + 0x12) = (unsigned int)user->getCurCharacNo();    // local_43
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
