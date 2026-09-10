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

    // ORIG 084d7b59 +0x091 `movzbl 0x4(%eax),%eax; test %al,%al; je +0xaa`（跳到 $0 块）
    //   ⇒ fall-through 分支落 `movl $0x1`，原码为 `!= 0` 在前。
    if (sig->m_flag != 0)
        packet.put_byte(1);
    else
        packet.put_byte(0);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);

    // P0 去具名局部：ORIG 084d7bb7..084d7bd8 两个字符串直接落 %esi/%ebx（无栈槽往返，我方原多 8 字节）
    //   ⇒ 原码是调用点内联三目；ORIG 先算 state（最后实参）再算 serverName，正是调用实参自右向左的求值序。
    cMyTrace(__PRETTY_FUNCTION__,
        0x3dea, 0)("%s server %s! SEND SIG_AUCTION_NOTIFY_AUCTION_SERVICE TO ALL USERS",
          sig->m_result == 0 ? "Auction" : "Point",
          sig->m_flag != 0 ? "Opened" : "Closed");
    return 0;
