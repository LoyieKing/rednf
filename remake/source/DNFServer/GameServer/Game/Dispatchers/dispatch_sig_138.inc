int Dispatcher_AuctionSearchByItemKey::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // [b23S3C] 迁真类 PCK_AUCTION_SEARCH_BY_ITEMKEY_GA/GP（ORIG ctor 0x0822e6aa/
    // 0x0822ea84，P167 实证原 TU 镜像 ctor DspAuctionSearchByItemKeyPckC1Ei 为发明
    // 符号违 §4.1）；roi keys 经 m_search.m_roi.m_keys（ORIG get_short(Rt&)×2 形）。
    unsigned char channel;               // local_2d -0x29
    int channelIdx;                      // local_1c -0x18

    // +0x0a：读拍卖频道
    if (!packet.get_byte(channel))
        return LineFunc(0xc690,
                        __PRETTY_FUNCTION__,
                        0, 0);
    // +0x4c：channel > 1 → 拒绝
    if (1 < channel)
        return LineFunc(0xc693,
                        __PRETTY_FUNCTION__,
                        0, 0);
    channelIdx = (int)(unsigned int)channel;
    // +0x84：服务器未连接 → 错误包（PAY_TYPE=channel）
    if (IsAuctionServerDisconnected(channel))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbd, 0x8c, (PAY_TYPE)channelIdx);
        return 0;
    }
    // +0xc5：state 门禁。[G1Csig] OUR-only store 实证：我方 `movb $0x1,-0x15(%ebp)` +
    // `movb $0x0,-0x15` + `cmpb $0x0,-0x15` 三条（diff our[60:65]）⇒ 具名 bool fail 占 1 槽、
    // 其后槽全体下移 4；ORIG 侧无该槽（直短路门禁）⇒ 去具名 bool。
    if (user->get_state() < 3 || user->getCurCharacR() == 0)
        return LineFunc(0xc6b7,
                        __PRETTY_FUNCTION__,
                        0, 0);

    if (channelIdx == 1)
    {
        // ---- 金币拍卖（GP；ORIG ctor 0x0822ea84，category 0x12）----
        PCK_AUCTION_SEARCH_BY_ITEMKEY_GP pck;
        pck.m_uid = user->GetUID();
        pck.m_characNo = user->getCurCharacNo();
        if (!packet.get_int(pck.m_search.m_itemId))                 // +0x1a
            return LineFunc(0xc6c0,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_byte(pck.m_search.m_b1e))                   // +0x1e
            return LineFunc(0xc6c1,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_byte(pck.m_search.m_b1f))                   // +0x1f
            return LineFunc(0xc6c2,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_byte(pck.m_search.m_b20))                   // +0x20
            return LineFunc(0xc6c3,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_short(*reinterpret_cast<unsigned short*>(
                &pck.m_search.m_s21)))                              // +0x21（ORIG GP 段 get_short(Rt&)）
            return LineFunc(0xc6c4,
                            __PRETTY_FUNCTION__,
                            0, 0);
        // +0x2bf：itemKey 数量 ≤ 0x14
        if (0x14 < pck.m_search.m_b20)
            return LineFunc(0xc6c7,
                            __PRETTY_FUNCTION__,
                            0, 0);
        {
            unsigned int key;                                       // local_34 -0x30
            for (int i = 0; i < (int)(unsigned int)pck.m_search.m_b20; i = i + 1)
            {
                if (!packet.get_int((unsigned int&)key))
                    return LineFunc(0xc6cc,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                pck.m_itemKeys[i] = key;
            }
        }
        GlobalData::s_cera_auction_proxy->SendPacket(&pck.m_header);
    }
    else
    {
        // ---- 点券拍卖（GA；ORIG ctor 0x0822e6aa，category 0）----
        PCK_AUCTION_SEARCH_BY_ITEMKEY_GA pck;
        pck.m_uid = user->GetUID();
        pck.m_characNo = user->getCurCharacNo();
        if (!packet.get_int(pck.m_search.m_itemId))                 // +0x1a
            return LineFunc(0xc6e1,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_byte(pck.m_search.m_b1e))                   // +0x1e
            return LineFunc(0xc6e2,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_byte(pck.m_search.m_b1f))                   // +0x1f
            return LineFunc(0xc6e3,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_byte(pck.m_search.m_b20))                   // +0x20
            return LineFunc(0xc6e4,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_short(*reinterpret_cast<unsigned short*>(
                &pck.m_search.m_s21)))                              // +0x21（GA 段 ORIG Rt&）
            return LineFunc(0xc6e5,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (0x14 < pck.m_search.m_b20)
            return LineFunc(0xc6e8,
                            __PRETTY_FUNCTION__,
                            0, 0);
        {
            unsigned int key;                                       // local_38 -0x34
            for (int i = 0; i < (int)(unsigned int)pck.m_search.m_b20; i = i + 1)
            {
                if (!packet.get_int((unsigned int&)key))
                    return LineFunc(0xc6ed,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                pck.m_itemKeys[i] = key;
            }
        }
        if (!packet.get_short(*reinterpret_cast<short*>(
                &pck.m_search.m_roi.m_keys[0])))                    // +0x23（ORIG 0x08215fd0 get_short(Rs&)）
            return LineFunc(0xc6f3,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_short(*reinterpret_cast<short*>(
                &pck.m_search.m_roi.m_keys[1])))                    // +0x25（ORIG 0x08216018 Rs&）
            return LineFunc(0xc6f5,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_short(*reinterpret_cast<short*>(
                &pck.m_search.m_roi.m_keys[2])))                    // +0x27（ORIG 0x08216060 Rs&）
            return LineFunc(0xc6f7,
                            __PRETTY_FUNCTION__,
                            0, 0);
        // +0x699：ROI_Category::_sort（this = 包+0x23）
        pck.m_search.m_roi._sort();
        if (!packet.get_byte(pck.m_b2f))                            // +0x2f
            return LineFunc(0xc6fd,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_byte(pck.m_b30))                            // +0x30
            return LineFunc(0xc6fe,
                            __PRETTY_FUNCTION__,
                            0, 0);
        GlobalData::s_auction_proxy->SendPacket(&pck.m_header);
    }

    // +0x751：GM 延迟 trace（static gmList[5]，ORIG _ZL6gmList d @0x093701d8）
    for (int i = 0; i <= 4; i = i + 1)
    {
        if (gmList[i] == user->getCurCharacNo())
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xc70f, 0);
            trace("Trace Auction Delay, %s(%d)",
                  "REQ AuctionSearchByItemKey", user->getCurCharacNo());
            break;
        }
    }
    return 0;
