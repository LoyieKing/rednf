int CTitleBook::putTitle(CUser* user, ENUM_ITEMSPACE space, int slot,
                         int itemIdx, ENUM_TITLE_BOOK_CATEGORY category,
                         int titleSlot)
{
    // b28d-solo-CTitleBook 终判（AE，本波归零）：p167 差集 CLEAR（33=33 调用）、
    // 335=335 条。已修 Inven_Item C1Ev（sret 直构）、send_equip(slot)、cMyTrace 临时链、
    // delete_item→bool、sectionIdx int 化（消 `lea` vs `add`）。本波修复残余 2 处装载
    // 时机差：invenType 局部 int→INVEN_TYPE（P211/P265 同型化）——GetInvenSlot 的
    // sret/invenType/slot 装载序回正 ORIG edx/eax/ecx，且 delete_item 不再提前把
    // invenType 载入 %eax 跨 3 个立即数保活；旧「P75/N7 封顶」判词作废。
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
    CInventory* inven = (CInventory*)user->getCurCharacInvenW();
    // b28d-solo-CTitleBook（P211/P265）：ORIG 实参同型直传（无 int→enum 转换中转）
    // ⇒ 局部用 INVEN_TYPE 而非 int。
    INVEN_TYPE invenType =
        (INVEN_TYPE)GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space);

    // b28d-solo-CTitleBook（P0/P183）：ORIG 0x086420b6 把 GetInvenSlot 的 sret
    // 直构进 invenItem 槽（-0x8d）——「先声明后赋值」会多发一次 Inven_Item::C1Ev
    //（p167 EXTRA +1）。
    Inven_Item invenItem = inven->GetInvenSlot(invenType, slot);
    // [b28d-solo-CTitleBook 盲区修复·真 bug] 同 getTitle：ORIG 0x086420d9 段
    // `cmpl $0,0x14(%ebp); je <0x086420ec = mov $0x2,%ebx; jmp 返回>` +
    // `cmp %eax,%edx; je <0x086420f6 = 继续>` ⇒ **itemIdx==0 也 return 2**。
    // 旧写在 itemIdx==0 时继续，与 ORIG 相反（jumpcheck 命中：ORIG je→+0x94 / 我方 je→+0x9e）。
    if ((itemIdx == 0) || (invenItem.m_addInfo != itemIdx))
    {
        return 2;
    }

    // P218：ORIG 0x08642115 先把 sectionIdx 载入 %esi 跨 G_CDataManager 调用保活
    //（`mov -0x24(%ebp),%esi; call; add $0xa794,%eax`）；unsigned 局部使 4.4 改为
    // 调用后重载（`lea 0xa794(%eax),%edx; mov -0x24(%ebp),%eax`）⇒ 索引局部 int 化。
    int sectionIdx = CAchievement::_getSectionIdx(category, titleSlot);
    if (sectionIdx != -1)
    {
        // ORIG 0x8642129：按值拷贝 sections[sectionIdx]（析构后判定），
        // 检查 +0x30 m_lockMessage 非空即锁定、返回 0xc。
        stTitleSection section =
            ((CTitleBookMgrRaw*)G_CDataManager())->m_titleBook.m_sections[sectionIdx];
        if (!section.m_lockMessage.empty())
        {
            return 0xc;
        }
    }
    if (user->get_charac_level() < 10)
    {
        return 8;
    }
    if ((category == 2) && (user->get_charac_level() < 0x46))
    {
        return 9;
    }
    if (!CTitleBook::_checkFitSlot(itemIdx, category, titleSlot))
    {
        return 2;
    }
    if (!book->_checkInsertTitle(category, titleSlot, &invenItem))
    {
        return 2;
    }
    if (!inven->delete_item(invenType, slot, 1, eItemDelReason_0x2b, true))
    {
        return 2;
    }
    if ((invenType == 0) && (slot == 0xb))
    {
        // ORIG 0x0864229a 传 slot 形参（0x10(%ebp)）而非立即数 0xb。
        user->send_equip(slot);
    }

    Inven_Item old = book->getTitleSlotR(category, titleSlot);
    if (old.m_addInfo != 0)
    {
        if (invenType == 1)
        {
            int ret = inven->insertItemIntoInventory(
                old, eItemAddReason_7, true, false);
            if (ret < 0)
            {
                // P227：ORIG 0x086423bb 在 cMyTrace ctor 前右起求 varargs 并跨
                // ctor 以 %esi/%ebx 保活 ⇒ 全表达式临时链形。
                cMyTrace(__PRETTY_FUNCTION__, 0x24f, 5)(
                    "insertItemIntoInventory failed(title) %s %d %d",
                    NumberToString(user->get_acc_id(), 0),
                    user->getCurCharacNo(), old.m_addInfo);
            }
        }
        else if (invenType == 0)
        {
            int ret = inven->insertItemIntoEquipment(
                old, eItemAddReason_7, true, false);
            if (ret < 0)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x256, 5)(
                    "insertItemIntoEquipment failed(title) %s %d %d",
                    NumberToString(user->get_acc_id(), 0),
                    user->getCurCharacNo(), old.m_addInfo);
            }
            else
            {
                user->send_equip(slot);
            }
        }
    }
    book->_insertTitle(category, titleSlot, &invenItem);
    return 0;
