int CInventory::GetRemainCapacity(INVEN_TYPE invenType, Inven_Item::ITEM_TYPE type) const
{
    // b28d-solo-CInventory：113/129，type switch 被 4.4 折 bitmask（ORIG 用跳表 jmp *%eax）+ push %ebx 差 ⇒ N18a/N82 封顶。
    // b28d-37 [N18a/N82 封顶，ORIG 0x84f?]：push %ebx 差 + 帧/槽残差（113/129）。
    // [H8InvenPvPWorld 2026-09-11 · N 判词定版] 父泳道读出 ORIG .rodata 0x8c93af8
    // 起 11 项跳表（case0→0x84fb6db / case1→0x84fb6dd / case2→0x84fb6df 累加体 /
    // case3→0x84fb717 / case4→0x84fb71b / case5..9→0x84fb71d logFormat 错误块 /
    // case10→0x84fb719），与本案 `case 0,1,3,4,10` 空 break + `case 2` 循环 +
    // default=logFormat(0x493) **语义完全一致**；ORIG 0x084fb68a `cmp $0x6;ja` 为
    // 外层跳表、内层 0x084fb6cd `cmp $0xa;ja` + `mov &data#2177f246(,%eax,4)` 为
    // 内层跳表。
    // [J8InvenPvPWorld.CInventoryWriter 2026-09-11 · 本轮已解，推翻上面的 N6/N18a 判词]
    // 旋钮：原写「5 个 case 标签堆叠共享一个 break」被 4.4 判为共享出口 ⇒ 位测试
    // （`mov $0x1;shl %cl;and mask` + push %ebx）；改为每 case 独立 `case N: break;`
    // （5 个独立叶块）后 4.4 立即改回跳转表，再把 `case 2` 循环体写在 case 1 与
    // case 3 之间（叶块物理序）⇒ our_insns 123 → **113 = ORIG**，gap 0，指令序列逐条
    // 一致。TU 口径由 diff 升 ae（strict identical 口径不做 .o 重定位归一：
    // 我方 `mov 0x294(,%eax,4),%eax` vs ORIG `mov 0x8c93adc(,%eax,4),%eax` 永不等
    // ⇒ 本件封顶在 ae，非 identical）。
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
            break;
        case 1:
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
        case 3:
            break;
        case 4:
            break;
        case 10:
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
}