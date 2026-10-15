int CTitleBook::getTitle(CUser* user, ENUM_TITLE_BOOK_CATEGORY category,
                         int slot, int itemIdx, ENUM_ITEMSPACE space,
                         int& outSlot)
{
    // b28d-solo-CTitleBook 终判：389=389、p167 差集 CLEAR（27=27 调用）⇒ AE。
    // b28d-solo-CTitleBook 本波复核：TU clean 口径 AE（near/diff 列表无本件）。
    // 本件修三处：① cMyTrace 全表达式临时链（P227，ORIG ctor 前右起求 varargs）；
    // ② GetLock 谓词内联（P198②，去具名 uchar 槽往返）；③ 装备分支补
    // `title.m_seal = 0`（ORIG 0x08642a49 `movb $0x0,-0x91(%ebp)`，真语义缺口）。
    CTitleBook* book = (CTitleBook*)user->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xe);
    if (!book)
    {
        return 3;
    }
    if (space == (ENUM_ITEMSPACE)0x12)
    {
        space = (ENUM_ITEMSPACE)3;
    }
    int result = checkError(user, category, slot);
    if (result != 0)
    {
        return result;
    }

    CInventory* inven = (CInventory*)user->getCurCharacInvenW();
    int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space);
    Inven_Item title = *book->_getTitle(category, slot);
    // [b28d-solo-CTitleBook 盲区修复·真 bug] ORIG 0x086426d1 段实证：
    //   `cmpl $0,0x14(%ebp)(itemIdx); je <0x086426e4 = mov $0x2,%eax; jmp 返回>` +
    //   `cmp %eax,%edx(m_addInfo); je <0x086426ee = 继续>` ⇒ 语义 = **itemIdx==0 也 return 2**。
    // 旧写 `(itemIdx != 0) && (title.m_addInfo != itemIdx)` 在 itemIdx==0 时继续执行，
    // 与 ORIG 相反（脚本/客户端传 itemIdx=0 时应拒绝）。AE 归一化把 jcc 目标折成 <T> 掩盖了
    // 该差；本轮「jmp/jcc 目标下标逐位核对」命中（ORIG je→+0x120 / 我方 je→+0x12a）。
    if ((itemIdx == 0) || (title.m_addInfo != itemIdx))
    {
        return 2;
    }
    CItem* pItem = G_CDataManager()->find_item(title.m_addInfo);
    if (!pItem)
    {
        return 2;
    }

    if (invenType == 1)
    {
        if (!CTitleBook::_checkMovetoInven(category, slot))
        {
            return 5;
        }
        if (inven->tryInsertItemIntoInventory(title) < 0)
        {
            return 0xb;
        }
        book->_deleteTitle(category, slot);
        int ret = inven->insertItemIntoInventory(
            title, eItemAddReason_7, true, false);
        if (ret < 0)
        {
            // P227：ORIG 0x08641e88 段在 cMyTrace ctor 前右起求 varargs 并跨
            // ctor 保活 ⇒ 全表达式临时链形。
            cMyTrace(__PRETTY_FUNCTION__, 0x2a5, 5)(
                "insertItemIntoInventory failed(title) %s %d %d",
                NumberToString(user->get_acc_id(), 0),
                user->getCurCharacNo(), title.m_addInfo);
        }
        outSlot = ret;
    }
    else if (invenType == 0)
    {
        if (outSlot != 0xb)
        {
            return 7;
        }
        if (inven->tryInsertItemIntoEquipment(title) < 0)
        {
            return 7;
        }
        if (IsEquipable(user, pItem, outSlot) != 0)
        {
            return 6;
        }
        // P198②：ORIG 0x08642a1b 段为 `call GetLock; test %al,%al; je`（直测返回
        // 寄存器、无槽）⇒ 谓词内联；提具名 uchar 局部会多 `mov %al,-off; cmpb $0`。
        if ((title.m_amp.GetLock() != 0) && (title.m_seal != 0))
        {
            return 1;
        }
        // 真语义缺口（ORIG 0x08642a49 `movb $0x0,-0x91(%ebp)`）：装备分支把称号
        // 移入背包前先清本地副本 title.m_seal（bool +0x00），再 _deleteTitle。
        title.m_seal = 0;
        book->_deleteTitle(category, slot);
        int ret = inven->insertItemIntoEquipment(
            title, eItemAddReason_7, true, false);
        if (ret < 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2c7, 5)(
                "insertItemIntoEquipment failed(title) %s %d %d",
                NumberToString(user->get_acc_id(), 0),
                user->getCurCharacNo(), title.m_addInfo);
        }
        user->send_equip(ret);
    }
    return 0;
}