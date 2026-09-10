bool CDBMsgSender::ChangeOwner(Inven_Item* item, int characNo, int slot,
                               int reason, int mapSize, int field6c, int field70)
{
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    reinterpret_cast<Inven_Item_Compatible*>((char*)buf + 4)->setCopy(*item);
    buf->m_addInfo2 = item->m_addInfo2;               // ← item+0x07
    buf->m_reason = (unsigned char)reason;            // +0x51
    buf->m_characNo = characNo;                       // +0x48
    buf->m_hdr |= 0x30;                               // or $0x30
    buf->m_slot = slot;                               // +0x2c
    buf->m_field64 = 1;
    buf->m_mapSize = mapSize;                         // +0x68
    buf->m_field6c = field6c;                         // +0x6c
    buf->m_field70 = field70;                         // +0x70
    return true;
