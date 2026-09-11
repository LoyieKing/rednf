// ORIG execute 0x084b293c：改写成长武器事件数据并落库——
//   标记置位：整体清零事件数据 + DB_EventGrowthWeaponReset(accId)；
//   否则：写时计值/清计数 + DB_EventUpdateGrowthWeaponTimepiece(accId, value, 0)。
void CGrowhWeaponEvent::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    GrowthWeaponEvent* data =
        static_cast<GrowthWeaponEvent*>(user->getGrowthWeaponEventdata());
    if (m_resetFlag)                 // [0x084b296c 字段直测，无具名局部 P0]
    {
        data->m_field8 = 0;
        data->m_field4 = 0;
        data->m_eventFlag = 0;
        data->m_field10 = 0;
        data->m_fieldc = 0;
        DB_EventGrowthWeaponReset::makeRequest(user->get_acc_id());
    }
    else
    {
        data->m_field10 = static_cast<short>(m_timepiece);   // [0x084b29b6 直读]
        data->m_fieldc = 0;
        DB_EventUpdateGrowthWeaponTimepiece::makeRequest(
            user->get_acc_id(),
            static_cast<short>((unsigned short)data->m_field10), 0);
    }
}