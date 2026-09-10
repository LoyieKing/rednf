int CInventory::check_item_exist(int itemIdx) const
{
    // ORIG 槽序(近→远)= type(-0xc) i(-0x10) start(-0x14) end(-0x18)（0x8505178/
    // 0x85051cd/0x850520c）；三个循环共用单槽 -0x10（4.1 scope 复用），
    // 改函数级具名 i 强制单槽。
    int start;
    int end;
    int i;
    Inven_Item::ITEM_TYPE type;
    for (i = 3; i < 9; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx)
        {
            return i;
        }
    }
    type = (Inven_Item::ITEM_TYPE)GetItemType(itemIdx);
    if (type == 0)
    {
        LogManager::logFormat(1, "inventory.cpp",
                              // ORIG 0x85051cf +0x5d: vararg 直落 mov -0xc(%ebp),%eax
                              // → 传 type 值而非 0。
                              __PRETTY_FUNCTION__, 0x1aa4,
                              "Unknown Type %d", type);
        // ORIG 0x8505202 +0x90: log 后 mov $0xffffffff,%eax; jmp end（体内早退）。
        return -1;
    }
    // ORIG 0x850520c +0x9a：无 start/end 初始化 dead store。
    StartEndPos(type, start, end);
    // ORIG 0x8505237 +0xc5: call IsCreatureItemType; test %al,%al; je +0x10a
    // → 正向判断 + creature(0x6e4) loop 为 fall-through 块（技巧 17/1 组合）。
    if (IsCreatureItemType(type))
    {
        for (i = start; i <= end; i++)
        {
            if (m_pCreatureSlot[i].m_addInfo == itemIdx)
            {
                return i;
            }
        }
        // ORIG 0x8505275 +0x103: creature 循环耗尽后本块自带 mov $0xffffffff
        // 尾岛（4.4 若合并到公共尾块会少一条）。
        return -1;
    }
    else
    {
        for (i = start; i <= end; i++)
        {
            if (m_pEquipSlot[i].m_addInfo == itemIdx)
            {
                return i;
            }
        }
    }
    return -1;
