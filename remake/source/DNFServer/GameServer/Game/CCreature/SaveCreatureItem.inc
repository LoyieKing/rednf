bool CCreature::SaveCreatureItem(Inven_Item* item,
                                 SIG_CREATURE_ITEM_DETAIL_INFO* info,
                                 CUser* user)
{
    (void)user;   // ORIG 未读该实参
    info->m_stomachValue = (int)m_stomach.GetStomachValue(
        (CREATURE_STATE)m_state, m_isGrow != 0);   // fnstcw/fldcw RC 截断
    info->m_exp = m_exp;                           // ← +0x30
    info->m_uid = m_uid;                           // ← +0x0c
    m_saveFlag = m_stomach.IsSaveStomach((CREATURE_STATE)m_state) ? 1 : 0;
    item->m_addInfo = reinterpret_cast<CCreatureItem*>(this)->GetItemId();
    info->m_slotNo = reinterpret_cast<CCreatureItem*>(this)->GetSlotNo();
    info->m_creatureItemType = (unsigned char)m_creatureItemType;  // 低字节 +0x08
    strncpy(info->m_name, m_name, 0xc);            // ← +0x1c，12B（ORIG call strncpy PLT）
    info->m_noCharge = m_noCharge;                 // +0x14 byte → +0x26
    info->m_stat = m_stat;                         // +0x15 byte → +0x27
    return true;
