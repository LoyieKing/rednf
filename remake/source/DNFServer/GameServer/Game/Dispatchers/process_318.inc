int Dispatcher_PurifyItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;  // ORIG 不读 param（local_18 仅赋值未使用）

    if (user->get_state() != 3)
    {
        return LineFunc(0xcbcb,
                        "virtual int Dispatcher_PurifyItem::process(CUser*, "
                        "MSG_BASE&, ParamBase&)",
                        0, 0);
    }

    MSG_PURIFY_ITEM& m = reinterpret_cast<MSG_PURIFY_ITEM&>(msg);

    if (G_GameWorld()->GetChannelType() == 7)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xcf, 0x11);
        return 0;
    }

    if (user->CheckItemLock(1, m.m_0d))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xcf, 0xd5);
        return 0;
    }

    try
    {
        Inven_Item targetItem;
        Inven_Item materialItem;

        const int purifyType =
            GlobalData::s_itemAmplifier_->getPurifyMaterialType(m.m_15);
        if (purifyType == -1)
        {
            throw static_cast<ENUM_ERROR>(0x11);
        }
        if (GlobalData::s_itemAmplifier_->getPurifyMaterialItemIdx(
                purifyType) != (int)m.m_15)
        {
            throw static_cast<ENUM_ERROR>(0x11);
        }

        targetItem =
            user->getCurCharacInvenR()->GetInvenSlot(1, m.m_0d);
        if (targetItem.m_addInfo != (int)m.m_0f)
        {
            throw static_cast<ENUM_ERROR>(0x11);
        }
        // 已鉴定（bit7==0）的强化选项不允许净化
        if (targetItem.m_amp.isIdentified())
        {
            throw static_cast<ENUM_ERROR>(0x17);
        }

        materialItem =
            user->getCurCharacInvenR()->GetInvenSlot(1, m.m_13);
        const int needCount =
            GlobalData::s_itemAmplifier_->getPurifyMaterialItemCount(
                purifyType);
        if (materialItem.m_addInfo != (int)m.m_15 ||
            materialItem.m_addInfo2 < needCount)
        {
            throw static_cast<ENUM_ERROR>(0x11);
        }

        CInventory* invenW = user->getCurCharacInvenW();
        if (!invenW->delete_item((INVEN_TYPE)1, m.m_13, needCount,
                                 (eItemDelReason)3, true))
        {
            throw static_cast<ENUM_ERROR>(0x16);
        }

        // ---- 内联：processPurifyType @0x082194b6 T ----
        // type 0：鉴定强化选项（stAmplifyOption_t::identify W 0x0822b3d8，
        //         未鉴定→清 bit7 返真；头内 void 声明，按指令语义展开）。
        // type 1：重置强化选项（stAmplifyOption_t::reset W 0x080cb75c）后返真。
        bool purified = false;
        if (purifyType == 0)
        {
            purified = !targetItem.m_amp.isIdentified();
            if (purified)
            {
                targetItem.m_amp.m_abilityType = (unsigned char)(
                    targetItem.m_amp.m_abilityType & 0x7f);
            }
        }
        else if (purifyType == 1)
        {
            targetItem.m_amp.reset();
            purified = true;
        }
        if (!purified)
        {
            throw static_cast<ENUM_ERROR>(0x16);
        }

        user->m_historyLog.apply_Item(materialItem.m_addInfo,
                                      targetItem.m_addInfo,
                                      (eApplyItemReason)2);

        invenW->update_item((INVEN_TYPE)1, m.m_0d, targetItem);

        // 刷新材料槽（删除后剩余数量入包）
        materialItem =
            user->getCurCharacInvenR()->GetInvenSlot(1, m.m_13);

        PacketGuard guard;
        guard.put_header(1, 0xcf);
        guard.put_byte(1);
        guard.put_short(m.m_13);
        guard.put_int(materialItem.m_addInfo2);
        guard.put_short(m.m_0d);
        guard.put_byte((unsigned char)targetItem.m_amp.getAbilityType());
        guard.put_short(targetItem.m_amp.getAbilityValue());
        guard.finalize(true);
        user->Send(guard);
        return 0;
    }
    catch (ENUM_ERROR& e)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xcf, (unsigned char)e);
        return 0;
    }
