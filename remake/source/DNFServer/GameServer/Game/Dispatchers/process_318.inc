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
        // [G1-Dispatchers] ORIG 两处独立调用 getPurifyMaterialItemCount：
        //   0x08219082（数量校验）与 0x082190d6（delete_item 实参位，结果入 %esi）
        // ⇒ 不设 needCount 具名局部（P48/P0；p167 ORIG-only ×1）。
        if (materialItem.m_addInfo != (int)m.m_15 ||
            materialItem.m_addInfo2 <
                GlobalData::s_itemAmplifier_->getPurifyMaterialItemCount(purifyType))
        {
            throw static_cast<ENUM_ERROR>(0x11);
        }

        // [G1-Dispatchers] P48：ORIG delete_item 与 update_item 各自重取
        // `getCurCharacInvenW()`（p167 ORIG-only ×1，我方原用 invenW 缓存）⇒ 不设具名 invenW。
        if (!user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1, m.m_13,
                GlobalData::s_itemAmplifier_->getPurifyMaterialItemCount(purifyType),
                (eItemDelReason)3, true))
        {
            throw static_cast<ENUM_ERROR>(0x16);
        }

        // [G1-Dispatchers] 还原 ORIG 0x082194b6 的**独立成员调用**
        // `_ZN21Dispatcher_PurifyItem17processPurifyTypeEiR10Inven_Item`（T）：
        // Dispatchers.h:388 已声明该成员、Game/Dispatchers/processPurifyType.inc 已有真实体
        // ⇒ 此处不得再内联展开（p167：ORIG-only processPurifyType x1，我方 EXTRA
        //   stAmplifyOption_t::isIdentified/reset 各 x1 即内联残留）。
        if (!processPurifyType(purifyType, targetItem))
        {
            throw static_cast<ENUM_ERROR>(0x16);
        }

        user->m_historyLog.apply_Item(materialItem.m_addInfo,
                                      targetItem.m_addInfo,
                                      (eApplyItemReason)2);

        user->getCurCharacInvenW()->update_item((INVEN_TYPE)1, m.m_0d, targetItem);

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
