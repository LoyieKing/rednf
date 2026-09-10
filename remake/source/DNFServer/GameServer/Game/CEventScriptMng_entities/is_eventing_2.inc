bool CPcRoomPlayTime::is_eventing()
{
    // ORIG 0x8110814：GetRepeatEvent(0x5c) 直接解引用判 IsEventing(0)
    // （ORIG 无空检）。
    if (CEventEntity::is_eventing())
    {
        if (GlobalData::s_event_manager->GetRepeatEvent(0x5c)->IsEventing(0))
            return true;
    }
    return false;
