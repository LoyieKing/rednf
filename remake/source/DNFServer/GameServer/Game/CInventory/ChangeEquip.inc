int CInventory::ChangeEquip(INVEN_TYPE invenType, int slot, int equipSlot)
{
    // b28d-solo-CInventory：124/123，两处 4.1 槽比零 setne 物化（P197④ 4.4 恒折直跳）+ 尾 nop ⇒ setcc-fold/N93附 封顶；P225 while+break 实测 127 条劣化已回退。
    // b28d-37 [setcc-fold + N93附 封顶，ORIG 0x084fc37e]：ORIG 两处 `cmpl $0,-N; setne %al; test` 为 4.1 槽比零物化（P197④ 4.4 恒折直跳）；具名 bool 局部实测槽化劣化；尾 nop 落点属 N93附。详注见 case 1/2 体。
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return 1;
    }
    switch (invenType)
    {
    case 1:
    {
        if (equipSlot <= 9)
        {
            return 0x15;
        }
        Inven_Item slotItem1 = GetInvenSlot(1, slot);
        if (slotItem1.m_seal != 0)
        {
            int secuResult = GlobalData::s_pSecuProtectionField->Check(m_pParent, (SECURITY_PROTCTION)0x16);
            if (secuResult != 0)
            {
                return secuResult;
            }
        }
        if (m_pParent != 0)
        {
            if (m_pParent->CheckItemLock(invenType, slot) &&
                GetInvenSlot(1, slot).m_seal != 0)
            {
                return 0xd5;
            }
        }
        int result = 0;
        result = move_item((INVEN_TYPE)0, equipSlot, (INVEN_TYPE)1, slot);
        // b28d-solo-CInventory [P225+P223]：ORIG `cmpl $0,-0x14; setne %al; test %al,%al;
        // je <nop 岛>` 为 4.1 槽比零物化（裸 if 4.4 恒折直跳）⇒ `while (r != 0) { return r; }`
        // 复现物化；两 case 各自的 `return 0` 在 ORIG 共享到函数尾且 case1 落 nop 岛、
        // case2 直落 mov$0（两独立 landing）⇒ 双 goto 尾标签串复现（探针 /tmp/probe_ce3.cpp fJ）。
        while (result != 0)
        {
            return result;
        }
        goto l_end1;
    }
    case 2:
    {
        if (equipSlot > 9)
        {
            return 0x15;
        }
        int result = 0;
        result = move_item((INVEN_TYPE)0, equipSlot, (INVEN_TYPE)2, slot);
        while (result != 0)
        {
            return result;
        }
        goto l_end2;
    }
    default:
    {
        goto l_end2;
    }
    }
l_end1:;
l_end2:;
    return 0;
}