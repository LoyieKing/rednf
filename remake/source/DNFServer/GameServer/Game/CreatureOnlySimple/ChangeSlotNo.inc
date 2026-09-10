bool CDBMsgSender::ChangeSlotNo(Inven_Item* item, int characNo, int slot,
                                int reason)
{
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    reinterpret_cast<Inven_Item_Compatible*>((char*)buf + 4)->setCopy(*item);
    buf->m_addInfo2 = item->m_addInfo2;               // ← item+0x07
    buf->m_reason = (unsigned char)reason;            // +0x51
    buf->m_characNo = characNo;                       // +0x48
    buf->m_hdr |= 0x20;                               // or $0x20
    buf->m_slot = slot;                               // +0x2c
    buf->m_field64 = 2;
    return true;
