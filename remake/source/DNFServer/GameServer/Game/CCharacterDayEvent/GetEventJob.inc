unsigned short CCharacterDayEvent::GetEventJob()
{
    // ORIG 0x08697886：movzwl 0xa(%eax),%eax 直返（unsigned short 返回无规范化）。
    return m_eventJob;
