void EventClassify::CEventScriptMng::test_process_change_day(int timeVal)
{
    if (timeVal <= 0x132daaf)
    {
        return;
    }

    m_todayTime = timeVal;                          // +0x48
    m_todayTimeDay = (unsigned char)(timeVal % 100);  // +0x4c

    std::map<unsigned short, CUser*>::iterator it =
        G_GameWorld()->m_UserInWorld.begin();       // +0x134 m_UserInWorld
    while (it != G_GameWorld()->m_UserInWorld.end())
    {
        CUser* user = it->second;
        if (user != 0 && user->get_state() >= 3)
        {
            user->RecoverFatigue(0);
            user->SendFatigue();
            static_cast<EventClassify::CEventScriptMng*>(
                G_CDataManager()->get_event_script_mng())
                ->send_event_init_data(user, true);
        }
        it++;
    }
