int CInventory::insertItemIntoInventory(Inven_Item item, eItemAddReason reason, bool bLog, bool b)
{
    if (item.m_addInfo == 0)
    {
        if (item.get_add_info() < 0)
        {
            return -1;
        }
        if (item.get_add_info() == 0)
        {
            return 0;
        }
        gain_money(item.get_add_info(), (eMoneyAddReason)7, bLog, 0);
        return 0;
    }
    if (reason != 7 && reason != 2)
    {
        // b28d-Inventory2：ORIG 0x08502e44 `mov -0x1c,%eax; cmp $0xffffffff,%eax`
        // （两段寄存器形）。unsigned 局部可复现三处 `mov -0x1c,%ecx/%ebx` 的
        // receiver 后装载位置；比较处保持 unsigned（改 signed 比较会使装载位置回退）。
        // 残余 1 条 `cmpl $0xffffffff,-0x1c`：4.4 对局部与常量比较恒折 mem 直比
        // （P209/N18b；P286 单 case switch、P310 int+cast 两形实测均不可翻）⇒ 封顶。
        // [实测回退] P310 形（int 局部 + 显式 (unsigned) 比较）在本件 d 3→5 **反劣**
        // （三处 receiver 装载位置连带变差）⇒ 维持 unsigned 局部形，残余 1 条 N18b 封顶。
        unsigned int dbLogType = (unsigned int)G_CDataManager()->IsDBLogItem(item.m_addInfo);
        if (dbLogType != (unsigned int)-1)
        {
            // [b28d-PoolMisc2 P251/P286 试验] ORIG 0x84f?（本段）为 4.1 的
            // `movzbl;movzbl %al,%eax; cmp $1,%eax; je; cmp $2,%eax; je; jmp`
            // **正向 switch 决策链**；`if/else if` 形 4.4 恒产 `cmp $N,%al; jne`
            // 反向链 ⇒ 改 switch 复现。
            switch (item.m_field1)
            {
            case 1:
                m_pParent->AddDBLogItem(item.m_addInfo, 1u,
                                        (ENUM_DBLOG_ITEM_TYPE)dbLogType);
                break;
            case 2:
                m_pParent->AddDBLogItem(item.m_addInfo, (unsigned int)item.get_add_info(),
                                        (ENUM_DBLOG_ITEM_TYPE)dbLogType);
                break;
            }
        }
    }
    if (item.IsCreatureItemType())
    {
        return insertItemIntoCreature(item, reason, bLog, b);
    }
    if (item.m_field1 == 8)
    {
        char ipgNo[24];
        WongWork::CGenUniqueNo::genIPGNo(
            WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE_6,
            ((CUserCharacInfo*)m_pParent)->getCurCharacNo(), ipgNo);
        int slot = m_pParent->addAvatarItem(item.m_addInfo, (char)0, (char)item.m_fieldb,
                                            ipgNo, (eAvatarItemAddReason)0x14);
        m_pParent->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)1, slot);
        return slot;
    }
    if (item.m_field1 != 1)
    {
        int result = stackInvenItemIntoQuickSlot(item, reason, bLog);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
        result = stackInvenItemIntoCorrespondingSpace(item, reason, bLog);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
    }
    if (item.m_field1 == 2)
    {
        int result = insertInvenItemIntoQuickSlot(item, reason, bLog);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
        result = insertInvenItemIntoCoresspondingSpace(item, reason, bLog);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
    }
    else
    {
        int result = insertInvenItemIntoCoresspondingSpace(item, reason, bLog);
        if (result >= 0)
        {
            return result;
        }
        result = insertInvenItemIntoQuickSlot(item, reason, bLog);
        if (result >= 0)
        {
            return result;
        }
    }
    return -1;
