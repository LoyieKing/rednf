int Timer_CreateDnfDbLoad::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    // ORIG 0x863ba46：GetRepeatEvent(0xa0)（CEventCreateDnf）判空 + IsEventing(0)
    // 为真时 RegistNextTimer()；随后无条件 DBSelectCreateDnfEventInfo::makeRequest()。
    CEventCreateDnf* event =
        static_cast<CEventCreateDnf*>(GlobalData::s_event_manager->GetRepeatEvent(0xa0));
    if (event != 0 && event->IsEventing(0))
    {
        event->RegistNextTimer();
    }
    DBSelectCreateDnfEventInfo::makeRequest();
    return 1;
