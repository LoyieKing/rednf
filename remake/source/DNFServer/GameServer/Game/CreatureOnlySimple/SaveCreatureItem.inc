bool CEgg::SaveCreatureItem(Inven_Item* item,
                            SIG_CREATURE_ITEM_DETAIL_INFO* info, CUser* user)
{
    (void)user;                     // ORIG 未读该实参
    item->m_addInfo = GetItemId();  // *(int*)(item+0x02) ← CCreatureItem::GetItemId()（ORIG call）
    info->m_uid = m_uid;            // info+0x04 ← this+0x0c
    info->m_slotNo = m_slotNo;      // info+0x00 ← this+0x10
    info->m_creatureItemType =
        (unsigned char)m_creatureItemType;                      // +0x24 低字节
    info->m_noCharge = m_noCharge;  // info+0x26 ← this+0x14（目标 signed char ⇒ ORIG 双寄存路由）
    info->m_stat = m_stat;          // +0x27 ← this+0x15
    return true;
