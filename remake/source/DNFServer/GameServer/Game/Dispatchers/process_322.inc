int Dispatcher_UseDye::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stUseDye& out = static_cast<stUseDye&>(param);
    MSG_USE_DYE& req = reinterpret_cast<MSG_USE_DYE&>(msg);

    int err = check_error(user, msg);
    out.m_4 = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1f3),
                                 static_cast<unsigned char>(out.m_4));
        return LineFunc(0x625a,
                        __PRETTY_FUNCTION__,
                        out.m_4, 0);
    }

    Inven_Item dyeItem;
    if (getItem(*user, 1, req.m_0e, dyeItem))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1f3),
                                 static_cast<unsigned char>(dyeItem.m_addInfo != 0));
        return 1;
    }
    Inven_Item targetItem;
    if (getItem(*user, 2, req.m_10, targetItem))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1f3), 1);
        return 1;
    }

    CItem* dyeDef = G_CDataManager()->find_item(dyeItem.m_addInfo);
    if (dyeDef == 0)
    {
        return 1;
    }
    if (!dyeDef->is_stackable())
    {
        return 1;
    }

    // ORIG 0x081e6bbc：经 CItem::GetDyeInfo()（0x0822c86e）取 DyeInfo 指针。
    // [Lane1Dispatchers 2026-09-10 · Main「视图 cast 反模式」修法] Lane41 已把
    //   DyeInfo::m_field0/m_vec4 private→public（含 sizeof==16 / offsetof(m_vec4)==4
    //   static_assert）⇒ 删除 TU 局部 UseDyeDyeInfoView + reinterpret_cast，改直访。
    const DyeInfo& dye = *dyeDef->GetDyeInfo();
    if (dye.m_field0 == 0 || dye.m_vec4.empty())
    {
        return 1;
    }
    // ORIG 0x081e6c0a/0x081e6c5c：非 const iterator（_Rb_tree_iterator::operator==）。
    std::map<int, AvatarVariation::colorRGB>::iterator colorIt =
        G_CDataManager()->m_avatarColor.m_colorMap.find(dye.m_vec4[0].first);
    if (colorIt == G_CDataManager()->m_avatarColor.m_colorMap.end())
    {
        return 1;
    }

    CEquipItem* avatarDef =
        static_cast<CEquipItem*>(G_CDataManager()->find_item(targetItem.m_addInfo));
    if (!avatarDef->IsAvatarItem())          // vtable +0x10 槽（P5 直调）
    {
        return 1;
    }
    if (avatarDef->IsAvatarColorVariation(0) != 1)
    {
        return 1;
    }

    // ORIG 以目标物品 idx 为 avatar key
    const WongWork::CAvatarItemMgr* mgrR =
        user->getCurCharacInvenR()->GetAvatarItemMgrR();
    const void* expansion = mgrR->GetExpansionInfo(targetItem.m_addInfo);
    if (expansion == 0)
    {
        return 1;
    }
    const short oldColorA = reinterpret_cast<const short*>(expansion)[0];
    const short oldColorB = reinterpret_cast<const short*>(expansion)[1];

    // ORIG 0x081e6d4c/0x081e6e2c：两处各自 getCurCharacInvenW()->GetAvatarItemMgrW()（P183）。
    const bool applied =
        user->getCurCharacInvenW()->GetAvatarItemMgrW()->SetAvatarColor(
            targetItem.m_addInfo, static_cast<short>(dye.m_vec4[0].first), 0);
    if (!applied)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1f3), 1);
        return LineFunc(0x6292,
                        __PRETTY_FUNCTION__,
                        1, user->get_acc_id());
    }

    const bool deleted = user->getCurCharacInvenW()->delete_item(
        static_cast<INVEN_TYPE>(1), req.m_0e, 1,
        static_cast<eItemDelReason>(3), true);
    if (!deleted)
    {
        // 删除失败：回滚头像颜色并报错
        user->getCurCharacInvenW()->GetAvatarItemMgrW()->SetAvatarColor(
            targetItem.m_addInfo, oldColorA, oldColorB);
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1f3), 0x11);
        return LineFunc(0x629b,
                        __PRETTY_FUNCTION__,
                        1, user->get_acc_id());
    }

    notifyUpdatedInventoryItemInfo(*user, req.m_0e);

    PacketGuard packet;
    packet.put_header(1, 0x1f3);
    packet.put_byte(1);
    packet.put_short(req.m_10);
    packet.put_int(4);
    {
        // pair.first 低 16 位 + 2 字节零填充（local_6c/local_6a）
        const unsigned char raw[4] = {
            static_cast<unsigned char>(dye.m_vec4[0].first & 0xff),
            static_cast<unsigned char>((dye.m_vec4[0].first >> 8) & 0xff),
            0, 0 };
        packet.put_binary(reinterpret_cast<const char*>(raw), 4);
    }
    packet.finalize(true);
    user->Send(packet);

    /* ORIG 0x81e6fa0..0x81e6fc8：发包后先通知头像变化
       SendAvatarEvent(0x31a, targetItem.m_addInfo, 颜色, 0, 0)，再取头像信息。 */
    user->getCurCharacInvenW()->SendAvatarEvent(
        0x31a, targetItem.m_addInfo, dye.m_vec4[0].first, 0, 0);

    /* ORIG 0x81e6fed..0x81e7062：GetAvatarInfo(targetItem.m_addInfo) →
       UseDye(dyeItem.m_addInfo, targetItem.m_addInfo, targetItem.m_addInfo,
       string(GetIPGAgencyNo()), req.m_10)（cUserHistoryLog @ user+0x79700）。 */
    WongWork::Avatar_Item* avatar =
        user->getCurCharacInvenW()->GetAvatarItemMgrR()->GetAvatarInfo(
            targetItem.m_addInfo);
    user->m_historyLog.UseDye(
        dyeItem.m_addInfo, targetItem.m_addInfo, targetItem.m_addInfo,
        std::string(avatar->GetIPGAgencyNo()), req.m_10);
    return 0;
