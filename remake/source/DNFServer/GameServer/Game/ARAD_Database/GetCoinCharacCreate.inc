int CEventMgr::GetCoinCharacCreate()
{
    int coin = 5;
    CEventBase* event =
        GlobalData::s_event_manager->GetRepeatEvent(5);
    if (event != 0 && event->IsEventing(static_cast<CUser*>(0)))
    {
        coin = reinterpret_cast<CCoinEventOnCharCreate*>(
                   event)->GetCoinNoOnCreateCharac();
    }
    return coin;
