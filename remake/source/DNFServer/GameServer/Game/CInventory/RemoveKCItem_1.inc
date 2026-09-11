void CInventory::RemoveKCItem(Inven_Item* inven, Inven_Item* equip)
{
    // b28d-solo-CInventory：142/146，同 RemoveFPItem（N108/P223 负例）封顶。
    // b28d-37 [N108/P223 负例，ORIG 0x850a0b7]：同 RemoveFPItem——我方多 2 个
    // jmp+nop 岛（4.4 不给『1 直落+2 岛』组合）；142/146。
    // ORIG 0x850a0b7..+0x4f：三条件 || 短路 je/jbe/ja → INC 岛(0x850a17f/
    // 0x850a182 4.1 nop)；i 槽 -0x18、pItem 槽 -0x14；addInfo 经 %ebx 跨
    // G_CDataManager 存活(0x850a0f7)。COND setle i<=0x19/i<=0x137。
    for (int i = 0; i <= 0x19; i++)
    {
        // b28d-solo-CInventory [P296+P223]：同 RemoveFPItem——正向包裹 + 两条独立
        // continue 复现「条件1 直落 + 条件2/3 nop 岛」（探针 /tmp/probe_island2.cpp fP）。
        if (inven[i].m_addInfo != 0)
        {
            if (inven[i].m_addInfo <= 0x1963) continue;
            if (inven[i].m_addInfo > 0x1b57) continue;
            CItem* pItem = G_CDataManager()->find_item(inven[i].m_addInfo);
            if (pItem == 0)
            {
                LogManager::logFormat(
                    1, "inventory.cpp",
                    __PRETTY_FUNCTION__, 0x23fe,
                    "CInventory::RemoveKCItem No EquipItem(%d) Found loopcount(%d)",
                    inven[i].m_addInfo, i);
            }
            else if (pItem->GetFinishPointPrice() != 0)
            {
                inven[i].reset();
            }
        }
    }
    for (int i = 3; i <= 0x137; i++)
    {
        if (equip[i].m_addInfo != 0)
        {
            if (equip[i].m_addInfo <= 0x1963) continue;
            if (equip[i].m_addInfo > 0x1b57) continue;
            CItem* pItem = G_CDataManager()->find_item(equip[i].m_addInfo);
            if (pItem == 0)
            {
                LogManager::logFormat(
                    1, "inventory.cpp",
                    __PRETTY_FUNCTION__, 0x2416,
                    "CInventory::RemoveKCItem No InvenItem(%d) Found loopcount(%d)",
                    equip[i].m_addInfo, i);
            }
            else if (pItem->GetFinishPointPrice() != 0)
            {
                equip[i].reset();
            }
        }
    }
}