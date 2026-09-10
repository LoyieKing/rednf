int Dispatcher_AuctionMyAuctionHistory::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    if (GlobalData::s_auction_proxy->IsRunning() != 1)
    {
        return 0;
    }
    if (user->get_state() < 3 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xc864,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    PCK_AUCTION_MY_AUCTION_HISTORY_GA ga;
    // ORIG ctor 仅填包头+memset 0x1e；三个 dword 无具名字段，按包体偏移写。
    // [b28d-solo-Dispatchers] P0：ORIG 0x082087xx 三个值各占具名槽
    //   （-0x14/-0x10/-0xc，`call; mov %eax,-0x14(%ebp)`）后再写包 ⇒ 补具名局部
    //   （直写字段形 4.4 会 lea 缓存字段地址到 %ebx）。
    // [b28d-solo-Dispatchers] P161/P235：ORIG 0x082087xx 三次 `call; mov %eax,
    //   -0x14/-0x10/-0xc(%ebp)` —— ga 基址 -0x26 加字段偏移 0x12/0x16/0x1a 恰为该
    //   三槽 ⇒ 原码直接写**具名载荷字段**（裸偏移/char 缓冲形 4.4 会 lea+%ebx 缓存
    //   地址）。字段已落 PCK_AUCTION_MY_AUCTION_HISTORY_GA.h（sizeof 0x1e 不变）。
    ga.m_uid = user->GetUID();
    ga.m_characNo = static_cast<unsigned int>(user->getCurCharacNo());
    ga.m_accId = user->get_acc_id();
    GlobalData::s_auction_proxy->SendPacket(reinterpret_cast<nsl::PACKET_HEADER*>(&ga));
    return 0;
