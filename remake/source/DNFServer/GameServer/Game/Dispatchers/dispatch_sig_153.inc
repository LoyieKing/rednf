int Dispatcher_AuctionSearchByNoItmeKey::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x082161e4..0x08216965 (0x782 字节)。GM 表 _ZL6gmList @0x093abb4c。

    unsigned char payType;   // ORIG 不预先初始化（get_byte 成功才写，失败即 return）
    if (packet.get_byte(payType) != 1)
        return LineFunc(0xc719, __PRETTY_FUNCTION__, 0, 0);
    if (payType > 1)   // cmp $0x1 / jbe
        return LineFunc(0xc71c, __PRETTY_FUNCTION__, 0, 0);

    const int pay = payType;   // movzbl 零扩展到 int
    if (IsAuctionServerDisconnected(payType))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbd, 0x8c, (PAY_TYPE)pay);
        return 0;
    }

    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
        return LineFunc(0xc73f, __PRETTY_FUNCTION__, 0, 0);

    // 包布局（ServerLab/Auction/AuctionServer/auction_header/AuctionPacket.h）：
    //   char_idx@0x12, character_id@0x16,
    //   TSearchByCategory@0x1a: startIndex@0x1a, category@0x1e, rarity@0x20,
    //   upgradeStart@0x21, upgradeEnd@0x22, lvStart@0x23, lvEnd@0x24,
    //   ROI_Search_category_key@0x25(12B，asm 仅填前 3 short), refineStart@0x31, refineEnd@0x32。
    if (pay == 1)
    {
        // PCK_AUCTION_SEARCH_BY_NOITEMKEY_GP ctor ORIG W 0x0822eafa：
        // PACKET_HEADER C1 + TSearchByCategory_ C1@0x1a + memset(this,0,0x33)
        // + setCategory(0x12) + setPacketID(7) + setSize(0x33)。
        // 直接读入包成员（权威头 PCK_AUCTION_SEARCH_BY_NOITEMKEY_GP：m_pad12@+0x12、
        // m_cat@+0x1a(T SearchByCategory_)）；不再用 char* body 临时指针/局部变量回填。
        PCK_AUCTION_SEARCH_BY_NOITEMKEY_GP pck;
        *reinterpret_cast<unsigned int*>(&pck.m_pad12[0]) = user->GetUID();
        *reinterpret_cast<int*>(&pck.m_pad12[4]) = user->getCurCharacNo();

        if (packet.get_int(*reinterpret_cast<unsigned int*>(&pck.m_cat.m_pad0[0])) != 1)
            return LineFunc(0xc748, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_short(*reinterpret_cast<unsigned short*>(&pck.m_cat.m_pad0[4])) != 1)
            return LineFunc(0xc749, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_byte(*reinterpret_cast<unsigned char*>(&pck.m_cat.m_pad0[6])) != 1)
            return LineFunc(0xc74a, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_byte(*reinterpret_cast<unsigned char*>(&pck.m_cat.m_pad0[7])) != 1)
            return LineFunc(0xc74b, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_byte(*reinterpret_cast<unsigned char*>(&pck.m_cat.m_pad0[8])) != 1)
            return LineFunc(0xc74c, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_byte(*reinterpret_cast<unsigned char*>(&pck.m_cat.m_pad0[9])) != 1)
            return LineFunc(0xc74d, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_byte(*reinterpret_cast<unsigned char*>(&pck.m_cat.m_pad0[0xa])) != 1)
            return LineFunc(0xc74e, __PRETTY_FUNCTION__, 0, 0);

        GlobalData::s_cera_auction_proxy->SendPacket(reinterpret_cast<nsl::PACKET_HEADER*>(&pck));
    }
    else
    {
        // PCK_AUCTION_SEARCH_BY_NOITEMKEY_GA ctor ORIG W 0x0822e736：
        // PACKET_HEADER C1 + TSearchByCategory_ C1@0x1a + memset(this,0,0x33)
        // + setCategory(0) + setPacketID(7) + setSize(0x33)。
        // 权威头 PCK_AUCTION_SEARCH_BY_NOITEMKEY_GA：m_data@+0x12、m_search@+0x1a
        // （含 m_roi ROI_Category@+0x25）、m_tail@+0x31。
        PCK_AUCTION_SEARCH_BY_NOITEMKEY_GA pck;
        *reinterpret_cast<unsigned int*>(&pck.m_data[0]) = user->GetUID();
        *reinterpret_cast<int*>(&pck.m_data[4]) = user->getCurCharacNo();

        if (packet.get_int(*reinterpret_cast<unsigned int*>(&pck.m_search.m_pad0[0])) != 1)
            return LineFunc(0xc760, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_short(*reinterpret_cast<unsigned short*>(&pck.m_search.m_pad0[4])) != 1)
            return LineFunc(0xc761, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_byte(*reinterpret_cast<unsigned char*>(&pck.m_search.m_pad0[6])) != 1)
            return LineFunc(0xc762, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_byte(*reinterpret_cast<unsigned char*>(&pck.m_search.m_pad0[7])) != 1)
            return LineFunc(0xc763, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_byte(*reinterpret_cast<unsigned char*>(&pck.m_search.m_pad0[8])) != 1)
            return LineFunc(0xc764, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_byte(*reinterpret_cast<unsigned char*>(&pck.m_search.m_pad0[9])) != 1)
            return LineFunc(0xc765, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_byte(*reinterpret_cast<unsigned char*>(&pck.m_search.m_pad0[0xa])) != 1)
            return LineFunc(0xc766, __PRETTY_FUNCTION__, 0, 0);

        // ROI 键：m_search.m_roi.m_keys[0..2] @ +0x25/+0x27/+0x29（ORIG get_short(short&)，
        // 以引用转型选中与 ORIG 相同重载，地址计算不变）。
        if (packet.get_short(*reinterpret_cast<short*>(&pck.m_search.m_roi.m_keys[0])) != 1)
            return LineFunc(0xc76a, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_short(*reinterpret_cast<short*>(&pck.m_search.m_roi.m_keys[1])) != 1)
            return LineFunc(0xc76c, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_short(*reinterpret_cast<short*>(&pck.m_search.m_roi.m_keys[2])) != 1)
            return LineFunc(0xc76e, __PRETTY_FUNCTION__, 0, 0);

        if (packet.get_byte(*reinterpret_cast<unsigned char*>(&pck.m_tail[0])) != 1)
            return LineFunc(0xc772, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_byte(*reinterpret_cast<unsigned char*>(&pck.m_tail[1])) != 1)
            return LineFunc(0xc773, __PRETTY_FUNCTION__, 0, 0);

        GlobalData::s_auction_proxy->SendPacket(reinterpret_cast<nsl::PACKET_HEADER*>(&pck));
    }

    // 尾部 GM 延迟追踪（_ZL6gmList ORIG @0x093abb4c，5 项，数据已 objdump 核实）
    static const int gmList[5] = { 0x4c2dc8, 0x5cf723, 0xa9c742, 0xa2ad85, 0x9ae1ae };
    for (int i = 0; i <= 4; i++)
    {
        if (gmList[i] == user->getCurCharacNo())
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0xc784, 0);
            trace("Trace Auction Delay, %s(%d)", "REQ AuctionSearchByNoItmeKey",
                  user->getCurCharacNo());
            break;
        }
    }
    return 0;
