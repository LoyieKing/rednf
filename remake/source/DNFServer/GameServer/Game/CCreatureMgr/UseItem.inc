// ORIG UseItem @ 0x833a5bc：feed 判定用 Inven_Item+2（int，即 m_addInfo/item idx）。
int CCreatureMgr::UseItem(Inven_Item* item, int slot)
{
    (void)slot;
    // ORIG 0x833a5cb/0x833a5dd：短路用直接分支（jne / xor;test;je），无 4.4
    // && 物化岛（mov1;jmp;mov0;test）⇒ 展开为嵌套 if，false 路径 fall-through。
    if (item->m_addInfo == 0x18)
    {
        if (!UseFeed())
            return 0x85;
    }
    if (m_equippedCreature != 0)
    {
        if (m_equippedCreature->vaildItemExp(item->m_addInfo))
        {
            if (!GainExp(item->m_addInfo))
                return 0x11;
        }
    }
    return 0;
}
