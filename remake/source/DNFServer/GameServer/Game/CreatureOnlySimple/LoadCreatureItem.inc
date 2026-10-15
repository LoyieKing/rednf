// ORIG _ZN13user_creature4CEgg16LoadCreatureItemEPNS_12CCreatureMgrEP10Inven_ItemP29SIG_CREATURE_ITEM_DETAIL_INFOP5CUser
// T 0x08338fac（LoadCreatureItem.md 全汇编转写）：蛋条目 DB 载入侧。
bool CEgg::LoadCreatureItem(CCreatureMgr* mgr, Inven_Item* item,
                            SIG_CREATURE_ITEM_DETAIL_INFO* info, CUser* user)
{
    if (info->m_stat != 0)          // +0x27 非 0 → 条目已删除标记
    {
        item->reset();              // ORIG call Inven_Item::reset(item)
        return false;
    }

    SetItemId(item->m_addInfo);     // *(int*)(item+0x02) 驱动脚本解析
    m_uid = info->m_uid;            // +0x0c ← info+0x04
    m_slotNo = info->m_slotNo;      // +0x10 ← info+0x00
    m_creatureItemType = (CREATURE_ITME_TYPE)(int)(char)info->m_creatureItemType;  // movsbl +0x24
    m_expireTime = info->m_expireTime;                          // +0x18 ← info+0x28
    item->m_seal = 1;             // *item = (Inven_Item)0x1（首字节置 1）
    m_noCharge = (info->m_noCharge != 0);                       // setne → +0x14
    m_stat = info->m_stat;          // +0x15 ← info+0x27

    // ORIG call CCreatureMgr::RegisterCreatureItem(this, info->m_uid)；
    // 返回 xor $1 后 je 失败路径 → !RegisterCreatureItem(...) 则返 0。
    if (!mgr->RegisterCreatureItem(reinterpret_cast<CCreatureItem*>(this),
                                   info->m_uid))
        return false;

    // 回填 CREATURE 区槽物：ref = getCurCharacInvenW(user)->GetInvenRef(3, slotNo)
    Inven_Item* ref = 0;
    ref =
        user->getCurCharacInvenW()->GetInvenRef(
            3, reinterpret_cast<CCreatureItem*>(this)->GetSlotNo());
    ref->m_seal = 1;              // ORIG movb $0x1,(%eax)
    ref->set_add_info(m_uid);       // ORIG call Inven_Item::set_add_info(ref, this+0x0c)
    return true;
}