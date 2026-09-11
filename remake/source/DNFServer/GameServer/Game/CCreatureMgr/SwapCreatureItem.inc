// ORIG _ZN13user_creature12CDBMsgSender16SwapCreatureItemEP5CUserP10Inven_ItemiS4_i
// W 0x0833fa0c（objdump 逐条转写）：m_stream 空 → 0；
// GetInBuffer<SIG_SWAP_CREATURE_ITEM> → memset 0x74；
// +0x04=slot1、+0x00=item1->get_add_info()、item1 空(+0x02==0) → +0x08=1；
// +0x10=slot2、+0x0c=item2->get_add_info()、item2 空 → +0x14=1；返回 1。
bool CDBMsgSender::SwapCreatureItem(CUser* user, Inven_Item* item1, int slot1,
                                    Inven_Item* item2, int slot2)
{
    (void)user;   // ORIG 未读该实参
    if (m_stream == 0)
        return false;
    SIG_SWAP_CREATURE_ITEM* buf = GetInBuffer<SIG_SWAP_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    buf->m_slot1 = slot1;
    buf->m_itemIdx1 = item1->get_add_info();
    // ORIG 直接读 item+0x02（== isEmpty 的判据）判空
    if (item1->m_addInfo == 0)
        buf->m_empty1 = 1;
    buf->m_slot2 = slot2;
    buf->m_itemIdx2 = item2->get_add_info();
    if (item2->m_addInfo == 0)
        buf->m_empty2 = 1;
    return true;
}
