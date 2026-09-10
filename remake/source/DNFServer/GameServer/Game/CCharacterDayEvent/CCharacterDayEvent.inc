CCharacterDayEvent::CCharacterDayEvent()
{
    // ORIG 0x0810a4c8：基类 CEventBase ctor + 写 vptr（编译器生成）；
    // movw $0xffff,0xa(%eax) 置 m_eventJob（-1 位模式）。
    m_eventJob = -1;
