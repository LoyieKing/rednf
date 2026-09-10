bool isOnEvent()
{
    // P183 禁指针缓存：ORIG 每步重物化，无具名局部（0x812cfac/cfbc 实证）。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x77)->IsEventing((CUser*)0))
        return false;
    return true;
