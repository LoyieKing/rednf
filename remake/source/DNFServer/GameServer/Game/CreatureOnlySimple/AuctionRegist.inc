bool CDBMsgSender::AuctionRegist(Inven_Item* item, int characNo, int unused14,
                                 int reason18, int mapSize)
{
    (void)unused14;   // ORIG 未读
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    reinterpret_cast<Inven_Item_Compatible*>((char*)buf + 4)->setCopy(*item);
    buf->m_addInfo2 = item->m_addInfo2;               // ← item+0x07
    buf->m_reason = (unsigned char)reason18;          // +0x51 ← p3(@0x18)
    buf->m_characNo = characNo;                       // +0x48
    buf->m_hdr |= 0x100;                              // or $0x1,%dh
    buf->m_field53 = 1;                               // +0x53
    buf->m_field64 = 5;
    buf->m_mapSize = mapSize;                         // +0x68
    return true;
