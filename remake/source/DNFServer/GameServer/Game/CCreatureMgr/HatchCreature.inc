bool CDBMsgSender::HatchCreature(int uid, int flag, int itemIdx)
{
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    buf->m_hdr |= 0x244;   // ORIG asm 833fcf0：or $0x244（0x200|0x40|0x4）
    buf->m_addInfo2 = uid;                            // +0x30
    buf->m_flag50 = (unsigned char)flag;              // +0x50
    // ORIG 0x833fd0f：无 compat 槽，`mov %edx,0x8(%eax)` buf 基址直存（具名视图折叠）。
    // [b28d-solo-CCreatureMgr P235] cast 形（base 先装载）改具名视角完整类型直访问。
    buf->m_itemView.m_itemIdx = itemIdx;              // buf+0x08
    CItem* item = G_CDataManager()->find_item(itemIdx);
    buf->m_days = item->getUsablePeriod();            // +0x54
    buf->m_expireTick = item->getExpirationDate();    // +0x58
    return true;
}
