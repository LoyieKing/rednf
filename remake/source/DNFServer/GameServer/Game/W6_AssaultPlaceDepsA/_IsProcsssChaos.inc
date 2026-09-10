bool CAssaultPlace::_IsProcsssChaos()
{
    if (GlobalData::s_event_manager->GetRepeatEvent(0x20)->IsEventing((CUser*)0))
        return false;
    return true;
