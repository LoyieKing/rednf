int Dispatcher_RequestAssaultPrice::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stRequestAssaultPrice& p = static_cast<stRequestAssaultPrice&>(param);
    MsgRequestAssaultPrice& m = static_cast<MsgRequestAssaultPrice&>(msg);

    CUser* target = G_GameWorld()->find_from_world((unsigned short)m.m_0d);
    if (target == 0)
    {
        p.m_4 = 3;
        return 0;
    }
    if (target->get_state() <= 2 || target->getCurCharacR() == 0)
    {
        p.m_4 = 3;
        return 0;
    }

    const int needMoney = pvp_assault::GetInstanceAssaultMgr()
        ->GetRequestAssaultNeedMoney(user, target);
    p.m_8 = needMoney;
    if (p.m_8 > 0)
    {
        // ORIG:GetInvenSlot(inven, 0, 0xb),取返回 Inven_Item 的 +0x02(item idx)查物品,
        // 命中则按装备折扣重算费用:money + discount*money/(-100.0f)(float 截断取整)。
        Inven_Item slot = user->getCurCharacInvenR()->GetInvenSlot(0, 0xb);
        CItem* item = G_CDataManager()->find_item(slot.m_addInfo);
        if (item != 0)
        {
            const float discount = static_cast<CEquipItem*>(item)->getUpgradeCostDiscount();
            const float base = (float)p.m_8;
            p.m_8 = (int)(base + (discount * base) / -100.0f);
        }
    }

    user->SetCurCharacAssaultMoney(p.m_8);
    user->SetCurCharacAssaultedUserId((unsigned short)target->get_unique_id());

    const int err = check_error(user, msg);
    p.m_4 = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x50f4, __PRETTY_FUNCTION__, err, 0);
    }
    return 0;
