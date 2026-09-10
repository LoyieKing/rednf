int CInventory::GetRemainCapacity(INVEN_TYPE invenType, Inven_Item::ITEM_TYPE type) const
{
    // b28d-solo-CInventory：113/129，type switch 被 4.4 折 bitmask（ORIG 用跳表 jmp *%eax）+ push %ebx 差 ⇒ N18a/N82 封顶。
    // b28d-37 [N18a/N82 封顶，ORIG 0x84f?]：push %ebx 差 + 帧/槽残差（113/129）。
    int count = 0;
    // [FL7GlobalData 2026-09-10 · P324 叶块物理序=书写序] ORIG 0x084f? 跳表后的
    // 首个叶块是 case 1（`movl $0x0,-0x1c; movl $0x0,-0x20`），而 `logFormat(0x4c3)`
    // 段落在函数**尾部**（ORIG 侧末 hunk）⇒ 原码 case 0/6 块写在最后。旧序把
    // case 0/6 写最前致叶块整体前移（我方首叶块即 logFormat）。
    switch (invenType)
    {
    case 1:
    {
        int start = 0;
        int end = 0;
        StartEndPos(type, start, end);
        switch (type)
        {
        case 0:
        case 1:
        case 3:
        case 4:
        case 10:
            break;
        case 2:
            for (int i = start; i <= end; i++)
            {
                if (m_pEquipSlot[i].m_addInfo == 0)
                {
                    count++;
                }
            }
            break;
        // [FL7GlobalData 2026-09-10 · P163 实测无效] ORIG 对 type switch 走完整跳表
        // （`cmp $0xa,%eax; ja <T>; mov <A>(,%eax,4),%eax; jmp *%eax`）；我方产
        // `1<<type & 0x41b` 位测试。补写 `case 5..9:` 与 default 共享体（语义等价）
        // 后 4.4 **仍产位测试**（ndiff 24 不变）⇒ 4.4 对本 case 集合恒选位测试，封顶。
        default:
            LogManager::logFormat(
                1, "inventory.cpp",
                __PRETTY_FUNCTION__,
                0x493, "Invalid Item Type");
            break;
        }
        break;
    }
    case 2:
        for (int i = 0; i < 0x69; i++)
        {
            if (m_pAvatarSlot[i].m_addInfo == 0)
            {
                count++;
            }
        }
        break;
    case 3:
    {
        int start = 0;
        int end = 0;
        StartEndPos(type, start, end);
        for (int i = start; i <= end; i++)
        {
            if (m_pCreatureSlot[i].m_addInfo == 0)
            {
                count++;
            }
        }
        break;
    }
    case 0:
    case 6:
        LogManager::logFormat(
            1, "inventory.cpp",
            __PRETTY_FUNCTION__,
            0x4c3, "Invalid inventory type");
        break;
    }
    return count;
