// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08214e44 orig/ours=210/224 ndiff-blocks=35
//   帧差 sub $60 vs $70 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x60,%esp | lea    -0x56(%ebp),%eax | mov    %eax,-0x44(%ebp)
int Dispatcher_AuctionBuyItemApiece::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08214e44: AuctionPreCheck(0x14f, PAY_TYPE 0)
    int result = user->AuctionPreCheck(static_cast<ENUM_CMDPACKET>(0x14f),
                                       static_cast<PAY_TYPE>(0));
    if (result >= 0)
        return result;

    // +0x03a: PCK_AUCTION_BUY_ITEM_APIECE_GA 请求包
    PCK_AUCTION_BUY_ITEM_APIECE_GA pck;
    // +0x045: ORIG [21]/[25] `mov %eax,-0x44(%ebp)` / `mov %eax,-0x40(%ebp)`
    //   —— 两个具名 int 暂存槽；直接用 lea 链写包体令 4.4 产 `lea 0x12(%eax),%ebx`
    //   三重 lea 且帧 0x60→0x70。
    // [G5DsigE] ORIG 直证 uid/characNo 落 pck 成员位（无独立具名槽）⇒ 具名成员直写。
    pck.m_body.m_uid = user->GetUID();
    pck.m_body.m_characNo = user->getCurCharacNo();


    // [V331b06 2026-09-12 ④ 修复] ORIG 把客户端包首个 int 直读进 pck+0x1a（= 权威
    //   AuctionPacket.h:222 `int price; // @26`），该字段随 SendPacket(&pck) 转发给
    //   AuctionServer（接收侧 HandlerFor_GA_.cpp:642 读 pPck->price）。原实现读进独立
    //   局部 `price` ⇒ 转发包 +0x1a 从未写入（恒 0），协议字段丢失。
    //   ORIG 0x8214e9f/0x8214ea2 `lea -0x56(%ebp); add $0x1a,%eax` → get_int(int&)。
    //   packed 成员不可直接绑定 int&（gcc 报「不能将紧实的字段绑定到 int&」）⇒ 与 m_count
    //   同法用具名 4 字节视图解引用（仍是具名成员访问，非裸偏移）。
    if (!packet.get_int(*reinterpret_cast<int*>(pck.m_body.m_price)))
    {
        return LineFunc(0xc501, __PRETTY_FUNCTION__, 0, 0);
    }
    // pck+0x26 = 竞拍单价/件数 int
    if (!packet.get_int(*reinterpret_cast<int*>(pck.m_body.m_count)))
    {
        return LineFunc(0xc502, __PRETTY_FUNCTION__, 0, 0);
    }
    // pck+0x1e = char[8] 竞拍键
    if (!packet.get_binary(pck.m_body.m_key, 8))

    {
        return LineFunc(0xc503, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x139: ORIG [72:76] `movl $0x0,-0x21/-0x1d/-0x19` + `movb $0x0,-0x15`
    //   —— 3×4B + 1×1B 立即数覆盖 13 字节；逐字节赋值被 4.4 折成 13 条 movb。
    // [G5DsigE] ORIG 4 条 disp 立即数 store = 聚合零初始化（13B < rep-stos 阈值）；
    //   裸指针 4 写被 4.4 展开成 lea+add+store 链（12 条）。
    char itemKey[13] = {0};

    if (!packet.get_binary(itemKey, 13))
    {
        return LineFunc(0xc508, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x1a0: 余额不足 → error 0xbc,0x90
    if (user->getCurCharacMoney() < *reinterpret_cast<int*>(pck.m_body.m_price))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xbc), 0x90);
        return 0;
    }

    // +0x1fc: use_money(price, 0x1a, true)
    if (!(user->getCurCharacInvenW()->use_money(
              *reinterpret_cast<int*>(pck.m_body.m_price),
              static_cast<eMoneySubReason>(0x1a), true)))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xbc), 0x93);
        return 0;
    }

    // +0x233: SaveMoneyCoin → 发 GA 包
    user->SaveMoneyCoin();
    GlobalData::s_auction_proxy->SendPacket(reinterpret_cast<nsl::PACKET_HEADER*>(&pck));

    // +0x24f: price > 4999 → 统计流 SIG_AUCTION_MONEY_SUB
    if (*reinterpret_cast<int*>(pck.m_body.m_price) > 4999)
    {
        Stream* stream = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0xc529);
        CStreamGuard guard(stream, true);
        **guard << 0x309;
        **guard << static_cast<int>(user->GetUID());
        SIG_AUCTION_MONEY_SUB* sig = guard->GetInBuffer<SIG_AUCTION_MONEY_SUB>();
        memset(sig, 0, 0x28);
        sig->m_arg04 = *reinterpret_cast<int*>(pck.m_body.m_price);

        strncpy(reinterpret_cast<char*>(sig) + 8, itemKey, 13);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), guard);
    }

    return 0;
}