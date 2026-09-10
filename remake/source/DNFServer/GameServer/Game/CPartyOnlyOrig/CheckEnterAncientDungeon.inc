int CParty::CheckEnterAncientDungeon(CDungeon const* dungeon)
{
    Inven_Item items[4];
    int slots[4];
    unsigned char flag;    // ORIG -0x1d：items 顶与 slots 底之间声明于函数作用域
    memset(items, 0, sizeof(items));
    memset(slots, 0, sizeof(slots));

    if (const_cast<CDungeon*>(dungeon)->m_entryRestrictFlag)
    {
        flag = 0;    // ORIG +0x79 分支内清零
        // b28d-solo-CPartyOnlyOrig：ORIG +0x0a7 `xor $0x1,%eax; test %al,%al;
        // je <mov $1>`（P194 `!=1` 形；CheckEnterEventDungeon 返回 bool）。
        // 旧 `==1?1:0` 产 `test %al;je` 直跳（少 xor）。
        if (CheckEnterEventDungeon(dungeon, flag, items, slots) != 1)
        {
            return 0;
        }
        return 1;
    }

    if (dungeon->m_entryRequiredItem.m_itemIdx == -1)
    {
        return 1;
    }
    if (!dungeon->m_entryRequiredItem.m_enableFlag)
    {
        return 1;
    }
    if (m_battleField.m_bloodState == 1)
    {
        return 1;
    }
    // b28d-solo-CPartyOnlyOrig：ORIG 0x0859e8f8/0x0859e8ff 双 `movl $0,-0xc`
    // （4.1 声明初始化 + for 初始化各一条，P191 配套）。
    int i = 0;
    for (i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        // [b23S3C] P48 去缓存：ORIG 两用点各自 call getCurCharacInvenR
        // （+0x163 / +0x1b4 两次独立 call，结果 eax 直落实参槽无 inv 槽）。
        slots[i] =
            reinterpret_cast<CUserCharacInfo*>(m_member[i].m_pUser)
                ->getCurCharacInvenR()
                ->check_item_exist(dungeon->m_entryRequiredItem.m_itemIdx);
        // b28d-solo-CPartyOnlyOrig：ORIG 0x0859e97c 为 `cmp $-1; sete %al;
        // test %al,%al; je`（4.1 槽比物化）；4.4 的 `if` 直形恒折 `jne`，
        // `while (cond) { return X; }` 形逐条复现（P225 扩展，探针 p22 g6）。
        while (slots[i] == -1)
        {
            return 0;
        }
        items[i] =
            reinterpret_cast<CUserCharacInfo*>(m_member[i].m_pUser)
                ->getCurCharacInvenR()
                ->GetInvenSlot(1, slots[i]);
        if (items[i].m_addInfo2 < dungeon->m_entryRequiredItem.m_itemCount)
        {
            return 0;
        }
    }
    return 1;
