int CUser::change_equip(int space, short slot, int dstSlot)
{
    int ret = 0;
    // ORIG 决策链 mov 0xc(%ebp),%eax / cmp $1 / cmp $2 / test ⇒ switch；
    // case 叶块物理序 = 书写序（P324）：0 → 2 → 1，default 收尾。
    switch (space)
    {
    case 0:
        if (G_GameWorld()->IsCheckUnusableEquipmentChannel())
        {
            const Inven_Item* item =
                getCurCharacInvenR()->GetInvenRef(1, slot);
            if (!online_preliminary::COnlinePreliminary::CheckUnusableEquipment(
                    item))
            {
                SendNotiPacketMessage(
                    g_scriptStringManager_.findString(4, "game_server_msg_146",
                                                      (bool*)0),
                    (ENUM_MESSAGE_TYPE)0);
                return 1;
            }
        }
        // ORIG 两守卫各自 je 同一出口 + 首条件 sete 物化 ⇒ 嵌套 if
        //（合并 && 会退化成短路直跳形，P133/P225）。
        if (getCurChannelEquipslotSwitch() == 1)
        {
            if (dstSlot != 0xb)
            {
                // ORIG: call GetInvenRef → mov 0x2(%eax),%eax → %ebx →
                // call G_CDataManager → call find_item（item 无槽往返，
                // P0/P183：实参表达式内联，勿声明中间指针局部）。
                CItem* itemInfo = G_CDataManager()->find_item(
                    getCurCharacInvenR()->GetInvenRef(1, slot)->m_addInfo);
                if (!(itemInfo == 0 ||
                      itemInfo->IsEnableWorld((ENUM_WORLD_TYPE)1)))
                    return 1;
            }
        }

        ret = getCurCharacInvenW()->ChangeEquip(INVEN_TYPE_EQUIP, slot,
                                                dstSlot);
        if (ret > 0)
            return ret;
        break;

    case 2:
        if (CheckInTrade())
            return 0x13;
        break;

    case 1:
        if (G_GameWorld()->IsCheckUnusableEquipmentChannel())
        {
            const Inven_Item* item = getCurCharacInvenR()->GetInvenRef(2, slot);
            if (!online_preliminary::COnlinePreliminary::CheckUnusableEquipment(
                    item))
            {
                SendNotiPacketMessage(
                    g_scriptStringManager_.findString(4, "game_server_msg_146",
                                                      (bool*)0),
                    (ENUM_MESSAGE_TYPE)0);
                return 1;
            }
        }
        if (isCompetitionMercenary())
            return 0xb3;

        ret = getCurCharacInvenW()->ChangeEquip(INVEN_TYPE_AVATAR, slot,
                                                dstSlot);
        if (ret == 0)
            setAvatarTypeBonus(
                static_cast<char>(CalcMercenaryAvatarBonus()));  // movsbl %al
        if (ret > 0)
            return ret;

        ARAD::Singleton<AvatarRechargeServer>::Get()->CalcDurabilityChangeEquip(
            this, dstSlot);
        break;

    default:
        return 1;
    }

    if (dstSlot >= 0 && dstSlot < 12)
        m_characterView.enableSaveCharacView();
    send_equip(dstSlot);
    return 0;
