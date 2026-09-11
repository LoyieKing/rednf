// ---- OnEnchantByBead（报告，ORIG T 0x0849ed1a sz 0x474）----
// b28d-solo-ExpertJobOnlyMgr：N 封顶（N82附/N115/N7）+ 真语义修复。
// 修复：①**真 bug**——统计调用漏第二次 `masterItem->get_index()`
//   （ORIG 0x0849f076 为第二次独立调用，P48；p167 修前 MISSING 1 → 修后 46/46
//   CLEAR）；②m_fieldd 写入时机提前到发包前（ORIG 0x0849f288）且 EnchantItem
//   第 3 参**重读** ringRef->m_fieldd（无 replacedCardIdx 具名局部，P0/P48）；
//   ③cMyTrace 改 `__PRETTY_FUNCTION__` + 全表达式临时链（P118/P139/P227）。
// 残差：帧 0x60 vs ORIG 0x6c（4.4 带 call 帧恒 16k 量化，N115/P190②不可凑）+
// 缺 push %edi（ORIG 用 %edi 存 &user->m_historyLog，4.4 用 %esi）⇒ 全帧槽号
// 整体差 0x10；callee-saved 角色互换（N7/N82附）。指令集合/调用点全等。
// [L3Free25to36 2026-09-11 复核·负结论] 逐槽核账：两侧**槽使用计数表完全相同**
// （16/2/3/9/5/5/2/6/2/2 次）且相对间距全等（packet 对象 0xc、其后各槽间距一致）
// ⇒ 局部变量集与物化点一致，唯一差异是整体 0x10 平移 + `push %edi`（ORIG idx2，
// 我方无）。另核：ORIG 该件帧 0x6c 用 4 个 push（16B）、我方 0x60 用 3 个（12B），
// %.esp 对齐两侧均满足 ⇒ 「16k 量化」不是帧差主因，真正缺的是 ORIG 在 %ebp 与首个
// 被引用槽之间的 16B 不可见分配（-0x1c..-0x4 无任何 %ebp 引用）。
// 试形记录：补 16B 死成员会令帧 0x60→0x70（>ORIG 0x6c）且仍无 `push %edi`，
// 属布局产物、无源形可择 ⇒ 维持封顶（N115/N7）。
int CExpertJobMgr::OnEnchantByBead(CUser* user, int param2, int beadSlot,
                                   int param4, int ringSlot)
{
    (void)param2;
    (void)param4;

    Inven_Item* beadRef = user->getCurCharacInvenW()->GetInvenRef(1, beadSlot);
    if (beadRef == 0)
    {
        return 0x11;
    }
    Inven_Item* ringRef = user->getCurCharacInvenW()->GetInvenRef(1, ringSlot);
    if (ringRef == 0)
    {
        return 0x11;
    }

    // 租赁两连查（ORIG 0x82702dc 实证：查的是 ringRef 的 m_addInfo，kind=1/2）
    if (GlobalData::s_rentalSystem->CheckRentableItem(
            1, (unsigned int)ringRef->m_addInfo))
    {
        return 0x11;
    }
    if (GlobalData::s_rentalSystem->CheckRentableItem(
            2, (unsigned int)ringRef->m_addInfo))
    {
        return 0x11;
    }

    // P48：ORIG G_CDataManager() 每用点 fresh（无 dm 槽）
    CItem* beadItem = G_CDataManager()->find_item(beadRef->m_addInfo);
    if (beadItem == 0)
    {
        return 0x11;
    }
    CItem* ringItem = G_CDataManager()->find_item(ringRef->m_addInfo);
    if (ringItem == 0)
    {
        return 0x11;
    }
    if (!beadItem->is_stackable())
    {
        return 0x11;
    }

    // 怪物卡主表链（ORIG 序）：cardId→masterItem→EnableWorld 校验
    int cardId = ((CStackableItem*)beadItem)->GetMonsterCardId();
    CItem* masterItem = G_CDataManager()->find_item(cardId);
    if (masterItem == 0)
    {
        return 0x11;
    }
    if (EnableWorld(ringItem, beadItem) != 1)
    {
        return 0x13;
    }
    // ORIG 0x849eeb3：ringItem->GetItemType() 结果作实参调
    // masterItem->getUsableItemType()（虚槽 vptr+0xc / vptr+0x20），失败返回 0x17。
    if (!masterItem->getUsableItemType(ringItem->GetItemType()))
    {
        return 0x17;
    }

    bool deleted =
        user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, beadSlot, 1,
                                                (eItemDelReason)8, true);
    if (!deleted)
    {
        // b28d-solo（P118/P139/P227）：ORIG 0x0849ef5f ctor 实参为
        // __PRETTY_FUNCTION__ 局部静态符号，且 getCurCharacNo() 在 ctor **前**
        // 求值并入 %ebx 跨 ctor 保活（全表达式临时链）⇒ 不用具名 trace 对象。
        cMyTrace(__PRETTY_FUNCTION__, 0x6cd, 5)(
            "CExpertJobMgr::OnEnchantByBead, getCurCharacInvenW()->delete_item "
            "failed , %d %d", user->getCurCharacNo(), beadSlot);
        return 0x11;
    }

    int savedCardIdx = ringRef->m_fieldd;
    // b28d-solo：ORIG 0x0849f076 先写 ringRef->m_fieldd = masterItem->get_index()
    //（第一次调用），无 replacedCardIdx 具名局部（P0 无槽往返）。
    ringRef->m_fieldd = masterItem->get_index();

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xe);
    packet.put_byte(0);
    packet.put_short(2);
    user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, ringSlot, packet);
    user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, beadSlot, packet);
    packet.finalize(true);
    user->Send(packet);

    // 统计（ORIG 0x860f940/0x860f9d4：increaseItemCount((ENUM_EXPERT_JOB_TYPE)1,
    // masterIndex, 1)）——ORIG 0x0849f076 为**第二次独立** get_index 调用（P48）。
    GetInstanceExpertJobStatistic()->increaseItemCount(
        (ENUM_EXPERT_JOB_TYPE)1, (unsigned long)masterItem->get_index(), 1);

    // 历史（ORIG 0x8684d74：EnchantItem(ringRef->m_addInfo, savedCardIdx,
    // ringRef->m_fieldd 重读, true, acc_name, characName)）
    user->m_historyLog.EnchantItem(ringRef->m_addInfo, savedCardIdx,
                                   ringRef->m_fieldd, true, user->get_acc_name(),
                                   user->getCurCharacName());

    packet.clear();
    packet.put_header(1, 0x113);
    packet.put_byte(1);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}