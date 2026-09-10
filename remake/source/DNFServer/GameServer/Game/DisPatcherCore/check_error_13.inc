int DisPatcher_BuyItem::check_error(CUser* user, MSG_BASE& msg)
{
    const MSG_BUY_ITEM& req = *reinterpret_cast<const MSG_BUY_ITEM*>(&msg);
    if (!G_CDataManager()->checkValidShopItem((unsigned int)req.m_deliveryType,
                                              (unsigned long)req.m_itemIdx))
    {
        // [Lane5] P47/P194 正极性：ORIG 0x081be4b6 `call; test %al,%al; je <hack块>`
        // + fall `mov $0x11` ⇒ 原码 `if (f()) return 0x11;`（负极性 ==0 生 xor $1）。
        if (G_CDataManager()->m_itemShop->IsOneADayShop(req.m_deliveryType))
        {
            return 0x11;
        }
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x3c9, 1, 0, 0);
        return -1;
    }
    if (user->CheckInTrade())
    {
        return 0x13;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0x13;
    }
    if (user->isHackUser())
    {
        return 0x13;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 3))
    {
        return 0xd1;
    }
    if (user->get_state() != 8)
    {
        // ORIG 0x081be5a8..0x081be5d0：Check 单次调用、结果存 -0xc(%ebp)，
        // 非 0 直接返回该码（我方原「==0 进主体 + 末尾重调」为双调用，p167 EXTRA +1）。
        int secuResult = GlobalData::s_pSecuProtectionField->Check(
            user, static_cast<SECURITY_PROTCTION>(0x22));
        if (secuResult != 0)
        {
            return secuResult;
        }
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        return 0x3c;
    }
    if (G_CDataManager()->m_itemShop->IsOneADayShop(req.m_deliveryType) != 0)
    {
        // ORIG vtable+0x34 槽直调 = IsEventing(CUser*)；对指针无空检。
        if (!GlobalData::s_event_manager->GetRepeatEvent(0x6d)->IsEventing(
                (CUser*)0))
        {
            return 0x11;
        }
    }
    return 0;
