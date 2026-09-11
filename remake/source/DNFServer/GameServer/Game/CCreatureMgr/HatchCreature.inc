// ============================================================================
// CDBMsgSender::HatchCreature @ 0x0833fcc2（W，objdump 逐条转写）
//   m_stream 空 → 0；GetInBuffer<SIG_CREATURE_ITEM> → memset 0x74 →
//   m_hdr|=0x400；+0x30(m_addInfo2)=uid；+0x50(m_flag50)=flag 低字节；
//   +0x08(m_item 兼容区 m_itemIdx)=itemIdx；返回 1。hdr 位推断同族。
// ============================================================================
// [b28d-23 NEAR·N76] +0x8 store 装载序：ORIG 0x833fd0f RHS-first vs 4.4 LHS-first（槽基址形）；探针 /tmp/p23_probe.cpp k2 证仅形参基址 RHS-first，槽基址恒 LHS-first ⇒ N76。ORIG 0x0833fcc2
// [b28d-solo-CCreatureMgr AE] P235 SIG_CREATURE_ITEM::m_item 匿名 union 具名视角 m_itemView 复现 +0x08 store RHS-first；ORIG 0x833fcc2
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
