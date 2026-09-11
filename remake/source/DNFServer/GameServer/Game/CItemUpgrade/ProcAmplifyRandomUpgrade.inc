// ============================================================================
// ProcAmplifyRandomUpgrade @ 0x854ac8a
//   处理随机增幅结果：写升级值、统计、日志、广播。
//   返回错误码：0=成功, 4=失败。
// [b28d-solo-CItemUpgrade 判词] AE（P227 全表达式临时链复现 %esi/%ebx 跨 ctor
//   保活；真 bug 3 处：find_item/DB itemId 误用 ticket→item、IsRandomOption 两臂互换）。
//   原 DIFF 186/192：
//   本轮修：trace ctor 移到 characNo/NumberToString 之后（ORIG 0x0854ac93 求值序）、
//   itemIdx/dm 内联、oldUpgrade 改 int 槽。残余 = ORIG 4.1 把 characNo/idStr/
//   characName/characNo2/m_addInfo 保活于 %esi/%ebx/%edi（push edi/esi/ebx），
//   4.4 恒槽化（register 提示实测零效 192=192）⇒ N82/N18a 封顶。
// ============================================================================
int CItemUpgrade::ProcAmplifyRandomUpgrade(CUser* user, Inven_Item& item,
                                           Inven_Item& ticket, int update,
                                           const char* msg)
{
    // [b28d-solo-CItemUpgrade] ORIG 0x0854ac93 求值序：characNo（→%esi）→
    // NumberToString(get_acc_id(),0)（→%ebx）→ cMyTrace ctor（-0x34）→ trace()，
    // 两个值均无槽往返（P0）⇒ P227 全表达式临时链（ctor 前求值 varargs 并跨
    // ctor 以 callee-saved 保活；具名局部形会被 4.4 槽化）。
    cMyTrace(__PRETTY_FUNCTION__, 0x82c, 0)(
        "AmplifyRandomUpgrade m_id:%s char_no(%d) update:%d",
        NumberToString(user->get_acc_id(), 0), user->get_charac_no(-1), update);

    // ORIG 0x0854ad0a 无 itemIdx/dm 具名局部（P0，内联实参）；源对象是
    // **item**（0x10(%ebp)，Ghidra param_2），不是 ticket（0x14）——真 bug 修正。
    CItem* itemScript = G_CDataManager()->find_item(item.m_addInfo);
    if (itemScript == 0)
        return 4;

    if (!item.m_amp.hasAbility())
        return 4;

    const upgrade_table_t* table =
        GlobalData::s_itemAmplifier_->getUpgradeInfoTable();
    // ORIG 0x0854ad73：oldUpgrade 落 **int** 槽 -0x1c（movzbl 后 mov %eax）。
    int oldUpgrade = item.GetUpgrade();
    item.SetUpgrade((unsigned char)update);

    GetInstanceValueStatistic()->AddValueStatistic(
        (VALUE_STATISTIC_FIELD)0xe, user, 0);

    // ORIG 每处 use 各自重调 GetUpgrade（P48 无 CSE）：newCheck/0xc/DB/UpgradeItem/guild×2。
    if (item.GetUpgrade() >= table->m_bodyView.m_named.m_noticeUpgradeLevel)
    {
        _NoticeUpgrade(user, item, true);
    }

    if (0xc < item.GetUpgrade())
    {
        // ORIG 0x0854ae0a 第二次 GetUpgrade 结果落 -0x3c 槽（求值序 R2L 的首个
        // 求值项），其余三值（characName/characNo2/item.m_addInfo）经
        // %esi/%ebx/%edi 保活无槽往返 ⇒ 全内联实参。
        DB_ReqItemUpgradeLog::makeRequest(
            (unsigned int)G_CEnvironment()->m_serverEnvir.m_serverGroup,
            item.m_addInfo, user->getCurCharacNo(), user->getCurCharacName(),
            item.GetUpgrade() & 0xff, true);
    }

    // ORIG 0x0854ae71 无 itemIndex 具名局部（P0）：get_index 结果直接在 %eax 传参，
    // 求值序 R2L（先 GetUpgrade→%ebx，后 get_index）。
    user->m_historyLog.UpgradeItem(itemScript->get_index(), oldUpgrade,
                                   item.GetUpgrade() & 0xff, true);

    // ORIG 0x0854aeb8 `je` 落 itemName 块（IsRandomOption()==false）⇒ true 用
    // **msg**、false 用 GetItemName()（原重建两臂互换 ⇒ 真 bug）。
    if (itemScript->IsRandomOption())
    {
        user->makeGuildItemUpgradMessage(msg, oldUpgrade, item.GetUpgrade() & 0xff, true);
    }
    else
    {
        // ORIG 0x0854af03 无 itemName 具名局部（P0）：GetItemName 结果直接传参。
        user->makeGuildItemUpgradMessage(itemScript->GetItemName(), oldUpgrade,
                                         item.GetUpgrade() & 0xff, true);
    }
    return 0;
}