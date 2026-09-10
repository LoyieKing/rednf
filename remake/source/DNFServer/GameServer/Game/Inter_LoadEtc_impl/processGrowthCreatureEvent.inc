void Inter_LoadEtc::processGrowthCreatureEvent(CUser& user)
{
    // 同 processGrowthWeaponEvent（ORIG 0x84e838c 同构）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x67)->IsEventing(0))
    {
        GiveGrowCreatureEvent* growth = static_cast<GiveGrowCreatureEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(0x67));
        if (growth != 0)
        {
            growth->SendGrowthCreatureEventInfo(user);      // ORIG T 0x081b6b70
        }
    }
