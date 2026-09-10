int Dispatcher_GrowthWeaponEventChangeInfinity::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stGrowthEquipment& out = static_cast<stGrowthEquipment&>(param);
    MSG_GROWTH_EQUIPMENT& req = reinterpret_cast<MSG_GROWTH_EQUIPMENT&>(msg);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x640f,
                        __PRETTY_FUNCTION__,
                        out.m_4, 0);
    }
    // ORIG _ZNK10CInventory11GetInvenRefEii（const 视图）。
    const Inven_Item* item = user->getCurCharacInvenR()->GetInvenRef(
        GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)req.m_0d), req.m_11);
    if (item == 0)
    {
        out.m_4 = 0x15;
        return 0;
    }
    if (item->m_addInfo == 0)   // ORIG mov 0x2(%eax),%eax
    {
        out.m_4 = 0x15;
        return 0;
    }
    CEventBase* repeatEvent = GlobalData::s_event_manager->GetRepeatEvent(0x64);
    // ORIG 无判空直接虚调 vtable+0x34（slot IsEventing）传 0。
    if (repeatEvent->IsEventing(0))
    {
        CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x64);
        if (event != 0)
        {
            out.m_4 = static_cast<GrowthEquipEvent*>(event)->SendInfinityGrowthWeapon(
                user, item, req.m_0d, req.m_11);
            return 0;
        }
        return 0;
    }
    return 0;
