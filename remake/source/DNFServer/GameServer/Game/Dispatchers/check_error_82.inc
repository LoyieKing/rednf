int Dispatcher_MonstercardBind::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() <= 2)
    {
        return -1;
    }

    MSG_MONSTERCARD_BIND& m = reinterpret_cast<MSG_MONSTERCARD_BIND&>(msg);

    // 卡 1（msg+0x0d）：须为非堆叠怪物卡且绑定数据索引有效
    // ORIG：每次 GetInvenSlot 前重新 getCurCharacInvenR()（不缓存指针）
    // [G1Frest 2026-09-10] ORIG 0x081d8e24 `movzwl 0xd(%eax),%eax; movswl %ax,%ebx`
    //   ⇒ 字段为 unsigned short（PacketTypes.h:877）而使用点按 **(short) 收窄**（P11）；
    //   旧码传裸 unsigned short 生 movzwl 零扩展（同族 0x081d8ec5/0x081d8fxx 三处）。
    Inven_Item slotA = user->getCurCharacInvenR()->GetInvenSlot(1, (int)(short)m.m_0d);
    CItem* cardA = G_CDataManager()->find_item(slotA.m_addInfo);
    if (cardA == 0)
    {
        return 0x11;
    }
    if (cardA->is_stackable())
    {
        return 0x11;
    }
    {
        // +0x330：怪物卡绑定数据索引（CStackableItem::m_int330；ORIG mov 0x330(%eax),%eax）
        const int bindInfoIdx =
            reinterpret_cast<const CStackableItem*>(cardA)->m_int330;
        if (bindInfoIdx == -1)
        {
            return 0x11;
        }
    }
    if (slotA.m_addInfo2 < 1)
    {
        return 0x16;
    }

    // 卡 2（msg+0x0f）：非堆叠 + 数量校验（无绑定索引检查，与 ORIG 一致）
    Inven_Item slotB = user->getCurCharacInvenR()->GetInvenSlot(1, (int)(short)m.m_0f);
    CItem* cardB = G_CDataManager()->find_item(slotB.m_addInfo);
    if (cardB == 0)
    {
        return 0x11;
    }
    if (cardB->is_stackable())
    {
        return 0x11;
    }
    if (slotB.m_addInfo2 < 1)
    {
        return 0x16;
    }

    // 卡 3（msg+0x11）：同卡 2 口径
    Inven_Item slotC = user->getCurCharacInvenR()->GetInvenSlot(1, (int)(short)m.m_11);
    CItem* cardC = G_CDataManager()->find_item(slotC.m_addInfo);
    if (cardC == 0)
    {
        return 0x11;
    }
    if (cardC->is_stackable())
    {
        return 0x11;
    }
    if (slotC.m_addInfo2 < 1)
    {
        return 0x16;
    }

    // 卡 2/卡 3 稀有度必须一致
    if (cardB->get_rarity() != cardC->get_rarity())
    {
        return 0x13;
    }

    // ---- check_need_slot @0x081d83c4 T ----
    if (!check_need_slot(user))
    {
        return 4;
    }

    // 非 PvP 通道且处于村庄态(3)才允许继续
    if (G_GameWorld()->IsPVPChannel() || user->get_state() != 3)
    {
        return -1;
    }

    // 类别校验（ORIG call CItem::hasCategory W 0x0822c97a）：两卡均须含类别 1，
    // 且均不得含类别 2。
    if (!(cardB->hasCategory((STItemScript::ENUM_ITEM_CATEGORY)1) &&
          cardC->hasCategory((STItemScript::ENUM_ITEM_CATEGORY)1)))
    {
        return 0x17;
    }
    if (cardB->hasCategory((STItemScript::ENUM_ITEM_CATEGORY)2) ||
        cardC->hasCategory((STItemScript::ENUM_ITEM_CATEGORY)2))
    {
        return 0x17;
    }

    // 保护场稀有度检查；非 0 错误码透传
    return GlobalData::s_pSecuProtectionField->CheckItemRarity(
        user, (SECURITY_PROTCTION)0x28, cardB, &slotB);
