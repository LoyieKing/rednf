int Inter_AuctionNotifyAuctionService::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
        reinterpret_cast<SIG_AUCTION_NOTIFY_AUCTION_SERVICE*>(data);

    PacketGuard packet;
    packet.put_header(0, 0xb7);
    packet.put_byte(sig->m_result);
    if (sig->m_result == 1)
    {   // 点数拍卖服
        GlobalData::s_cera_auction_proxy->SetRunning((char)sig->m_flag);
    }
    else
    {   // 金币拍卖服
        GlobalData::s_auction_proxy->SetRunning((char)sig->m_flag);
    }

    if (sig->m_flag == 0)
        packet.put_byte(0);
    else
        packet.put_byte(1);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);

    const char* state;
    if (sig->m_flag != 0)                              // ORIG 真分支直落 "Opened"
        state = "Opened";
    else
        state = "Closed";
    const char* serverName;
    if (sig->m_result == 0)                            // ORIG data==0 直落 "Auction"
        serverName = "Auction";
    else
        serverName = "Point";

    cMyTrace(__PRETTY_FUNCTION__,
        0x3dea, 0)("%s server %s! SEND SIG_AUCTION_NOTIFY_AUCTION_SERVICE TO ALL USERS",
          serverName, state);
    return 0;
