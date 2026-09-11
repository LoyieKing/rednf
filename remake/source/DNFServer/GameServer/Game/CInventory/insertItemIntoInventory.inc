// ORIG T 0x08502d86 (size 0x60b)。
// b28d-PoolMisc2 [P251/P286 **修成**：ndiff 18→7、426→427/428]：`dbLogType != -1`
// 块内 `if (m_field1==1) … else if (m_field1==2) …` 改 **switch (item.m_field1) {
// case 1/break; case 2/break; }** ⇒ 逐条复现 ORIG 0x08502e4c..0x08502e5d 的
// `movzbl 0xd(%ebp); movzbl %al,%eax; cmp $0x1,%eax; je; cmp $0x2,%eax; je; jmp`
// 正向决策链（原 if/else-if 形 4.4 恒产 `cmp $N,%al; jne` 反向链 + 两 case 体
// 错位，共 11 行残差归零）。余 7 行 = ① 0x08502e44 `mov -0x1c(%ebp),%eax;
// cmp $0xffffffff,%eax` vs 4.4 `cmpl $-1,-0x1c(%ebp)`（P75 mem-cmp 折叠）；
// ② 0x08502e67/0x08502ea? 的 `mov -0x1c(%ebp),%ecx/%ebx` 在 receiver 载入
// **之后**取槽（4.4 提前到实参链首，P211 档③装载时刻）⇒ 跨版本 codegen 封顶。
// [b28d-Inventory2 本轮] ② 已修：dbLogType 声明 unsigned int（比较处同步 unsigned）
// ⇒ 三处 `mov -0x1c,%ecx/%ebx` 落回 receiver 之后，gap 1→0 条数 428/427。
// ① 维持封顶：unsigned 局部 + P286 单 case switch + P310 int+cast 三形实测仍产
// `cmpl $-1,-0x1c`（4.4 对局部与常量比较恒 mem 直比，P209/N18b）⇒ 语义完成、
// byte 层余 1 条（p167 CALLSITE CLEAR 21/21）。
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
}