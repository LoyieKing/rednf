// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x0822997a orig/ours=470/457 ndiff-blocks=42
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: sub    $0x1cc,%esp | movb   $0x0,-0x4a(%ebp) | movl   $&_ZZN21Dispatcher_SelectItem12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp)
int Dispatcher_SelectItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0822997a：公平 PvP 装备包领取（PacketDispatcher_Impl_1.cpp）。
    // ORIG 0x08229986 首检直落 `mov 0xc(%ebp),%eax; mov %eax,(%esp); call get_state`，
    // 全函数 0 处 `cmpl $0x0,0xc(%ebp)`（判据①：`user == 0` 为发明校验，入口多两条）⇒ 删。
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
        return 0;

    if (G_GameWorld()->GetChannelType() != 0xf &&
        G_GameWorld()->GetChannelType() != 0x10)
        return 0;
    fair_pvp::CFairPvPScore* score =
        (fair_pvp::CFairPvPScore*)(void*)user->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)7);
    if ((int)score->GetGiveItem() >
        G_CDataManager()->m_serverParameter.get_pvp_item_give(user->get_pvp_grade()))
        return 0;

    unsigned char groupCnt = 0;
    if (!packet.get_byte(groupCnt))
    {
        return LineFunc(0xe785,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    int key = makeFairPVPEquipmentPackageKey(
        user->get_charac_job(), user->get_pvp_grade());

    // ORIG 0x0822997a 段：const_iterator 默认构造，循环条件每轮重取 get_pvp_grade()，
    // find/end 各自重取 G_CDataManager()（p167 ORIG-only G_CDataManager ×3）。
    std::map<int, std::vector<std::vector<STStackablePackageData> > >::const_iterator it;
    for (int attempt = 0; attempt <= user->get_pvp_grade(); ++attempt)
    {
        it = G_CDataManager()->m_stackableMap.find(key);
        if (it != G_CDataManager()->m_stackableMap.end())
            break;
        key = decreaseRankOfFairPVPEquipmentPackageKey(key);
    }
    if (it == G_CDataManager()->m_stackableMap.end())
        return 0;

    if ((unsigned int)it->second.size() != (unsigned int)groupCnt)
        return 0;

    std::vector<Inven_Item> items;
    for (std::vector<std::vector<STStackablePackageData> >::const_iterator
             grp = it->second.begin();
         grp != it->second.end(); ++grp)
    {
        unsigned char selIdx = 0;
        if (!packet.get_byte(selIdx))
        {
            return LineFunc(0xe7a1,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if ((unsigned int)grp->size() <= (unsigned int)selIdx)
        {
            return LineFunc(0xe7a4,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }

        // ORIG 用 vector::at（先 size 校验；p167 ORIG-only at ×2）。
        // [P4 负结论 2026-09-11 实测回滚] 按 P4ConstIterFamily 建议删缓存、两处直用 at()
        //   实测：ours 465→469 条（gap −4→0 改善）但 **fdiff blocks 49→61 劣化** ⇒ 按
        //   「ndiff 优先」判据回滚，维持单次 at + 引用缓存形。
        const STStackablePackageData& data = grp->at(selIdx);
        Inven_Item newItem;
        newItem.m_addInfo = data.m_field0;
        CItem* item = G_CDataManager()->find_item(data.m_field0);   // 推断：dispatch 域以基类指针持有
        if (item == 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x196, 0x11);
            return 0;
        }
        item->make_item(newItem);   // CItem vtable +0x08 槽（虚表实证）
        if (newItem.m_field1 != (unsigned char)Inven_Item::ITEM_TYPE_EQUIP)
        {
            newItem.m_addInfo2 = data.m_field4;
        }
        items.push_back(newItem);
    }

    for (std::vector<Inven_Item>::const_iterator itemIt = items.begin();
         itemIt != items.end(); ++itemIt)
    {
        int slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            *itemIt, (eItemAddReason)8, true, true);
        if (slot < 0)
        {
            char title[21];
            strncpy(title,
                    g_scriptStringManager_.findString(
                        4, "game_server_msg_171", (bool*)0),
                    sizeof(title));
            char msgBuf[0xff];
            strncpy(msgBuf,
                    g_scriptStringManager_.findString(
                        4, "game_server_msg_172", (bool*)0),
                    sizeof(msgBuf));
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                title, *itemIt, 0u, (unsigned int)user->getCurCharacNo(),
                msgBuf, (int)strlen(msgBuf), 0u, user->GetServerGroup(),
                false, false);
        }
        else
        {
            user->SendUpdateItemList(
                (CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, slot);
        }
    }

    // ORIG 结尾再次 GetCharacExpandData（p167 ORIG-only ×1）。
    ((fair_pvp::CFairPvPScore*)(void*)user->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)7))->SetGiveItem(user->get_pvp_grade() + 1);
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x196);
    return 0;
}