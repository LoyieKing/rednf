int CItemUpgrade_Separate::ProcUpgrade(CUser* user, MSG_ITEM_UPGRADE* msg,
                                       PacketGuard* packet)
{
    CInventory* invenW = user->getCurCharacInvenW();

    Inven_Item item = invenW->GetInvenSlot(INVEN_TYPE_EQUIP,
                                           msg->m_targetSlot);  // MSG_ITEM_UPGRADE +0x1b（movzwl 零扩展）
    if (item.isEmpty())
    {
        return 4;
    }
    if (item.m_addInfo !=        // Inven_Item +0x02
        msg->m_targetItemIdx)    // MSG_ITEM_UPGRADE +0x1d
    {
        return 4;
    }

    // ORIG 0x0811e6f5/0x0811e7e9：matItem 先 Inven_Item::C1Ev 默认构造（-0xf6），
    // 再由 GetInvenSlot 的 sret 临时（-0x138）逐字段隐式赋值拷贝。
    Inven_Item matItem;
    matItem = invenW->GetInvenSlot(INVEN_TYPE_EQUIP,
                                   msg->m_materialSlot);  // +0x23（movzwl 零扩展）
    if (matItem.isEmpty())
    {
        return 0x16;
    }

    // ORIG 0x0811e80d：item.m_addInfo 先入 %ebx 跨 G_CDataManager call 保活（P52 链式）。
    CItem* citem = G_CDataManager()->find_item(item.m_addInfo);
    if (citem == 0)
    {
        return 4;
    }

    const GenuineDamageUpgradeTable* table = getUpgradeInfoTable();
    STUpgradeInfo info;

    // ORIG 0x0811e850/0x0811e8a1：get_charac_level 两处内联独立调用（P48 无 CSE）。
    if (!_GetUpgradeInfo(&item, citem, *table, &info, user->get_charac_level()))
    {
        return 0xd;
    }

    // ORIG 0x0811e894 `movl $0x0,-0x28(%ebp)` 先于 call、0x0811e8de 再回填 ⇒
    // 原码是「声明=0 + 独立赋值」两句式（P198①，合写会丢预清行）。
    int cond = 0;
    cond = _CheckCondition(user, item, citem, matItem, *table,
                           user->get_charac_level());
    // [round5-C05 2026-09-10] P225：ORIG 0x0811e8de `cmpl $0x0,-0x28; setne %al;
    // test %al,%al; je` 是 4.1 的「bool 物化后条件分支」；4.4 对 `if (cond != 0)`
    // 恒折直跳（`je`，少 2 条）⇒ 改 while+break 形逐条复现。
    while (cond != 0)
    {
        return cond;
    }

    // T1 直证：ORIG 0x0811e905 `movzbl %al,%eax; mov %eax,-0x24` ⇒ 32 位无符号局部
    // （uchar 局部会生 `mov %al,-N` + 回读 movzbl，实测多 3 条）。
    const unsigned int sepOld =
        static_cast<unsigned char>(item.m_upgradeSep.GetUpgradeSeparate());

    // ORIG 0x0811e90b `movb $0x0,-0x1d(%ebp)` 预清 + 0x0811e932 回填（P198①）。
    bool success = false;
    success = _DoProcUpgrade(user, &item, &info);

    // P0：ORIG 0x0811e93d/0x0811e99e 每处重读 `msg->m_materialSlot`（movzwl 0x23），
    // 无 matSlot 具名局部；receiver 亦为链式右值（0x0811e947 无槽往返）。
    if (user->getCurCharacInvenW()->delete_item(INVEN_TYPE_EQUIP, msg->m_materialSlot,
                                                info.m_materialCnt,
                                                static_cast<eItemDelReason>(0xc),
                                                true) == 0)
    {
        return 0x11;
    }

    packet->put_byte(1);
    packet->put_short(msg->m_materialSlot);

    // ORIG 0x0811e94a：第三次 getCurCharacInvenW（receiver 右值直落）。
    Inven_Item refreshed =
        user->getCurCharacInvenW()->GetInvenSlot(INVEN_TYPE_EQUIP, msg->m_materialSlot);
    packet->put_int(refreshed.get_add_info());
    packet->put_byte(static_cast<char>(sepOld));

    if (static_cast<unsigned char>(item.m_upgradeSep.GetUpgradeSeparate()) > 5)
    {
        _NoticeUpgrade(user, item, success);
    }

    // ORIG 0x0811ea54：三目两臂各发独立 put_byte call（4.4 折单 call 丢 1 个调用点）。
    if (success)
    {
        packet->put_byte(0);
    }
    else
    {
        packet->put_byte(1);
    }

    // 同 sepOld：ORIG 0x0811ea93 `movzbl %al,%eax; mov %eax,-0x1c`。
    const unsigned int sepNow =
        static_cast<unsigned char>(item.m_upgradeSep.GetUpgradeSeparate());
    packet->put_byte(sepNow);

    // P0：ORIG 0x0811eaae/0x0811ead5 每处重读 `msg->m_targetSlot`（movzwl 0x1b）。
    packet->put_short(msg->m_targetSlot);

    if (item.m_addInfo != 0)   // Inven_Item +0x02
    {
        invenW->update_item(INVEN_TYPE_EQUIP, msg->m_targetSlot, item);
    }

    // ORIG 0x0811eb91：sepNow 实参内联 GetUpgradeSeparate（不用 -0x1c 变量）。
    user->m_historyLog.SeparateUpgradeItem(
        citem->get_index(), sepOld,
        static_cast<int>(item.m_upgradeSep.GetUpgradeSeparate()), success);

    // ORIG 0x0811ebd8/0x0811ec41：IsRandomOption 正极性两臂，各自内联完整实参
    // （makeGuild/GetUpgrade/GetUpgradeSeparate 各 2 次调用）。
    if (citem->IsRandomOption())
    {
        user->makeGuildItemSeparateUpgradMessage(
            msg->m_28, sepOld,
            static_cast<int>(item.m_upgradeSep.GetUpgradeSeparate()), success,
            item.GetUpgrade());
    }
    else
    {
        user->makeGuildItemSeparateUpgradMessage(
            citem->GetItemName(), sepOld,
            static_cast<int>(item.m_upgradeSep.GetUpgradeSeparate()), success,
            item.GetUpgrade());
    }
    return 0;
