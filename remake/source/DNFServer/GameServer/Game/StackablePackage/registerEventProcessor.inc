// ORIG 0x08281a2a (0x70)：无 null 检查。GetEventId 先求值存 -0x10，
// pair<const int,EventProcessor*> 直接双参构造（0x08281ffa C1IiRS3_EEOT_OT0_），
// 再 GetEventType 求值 -0xc，operator[] 取内层 map，insert(pair)。
void EventProcessManager::registerEventProcessor(EventProcessor* processor)
{
    m_impl->processors[processor->GetEventType()].insert(
        std::pair<int const, EventProcessor*>(processor->GetEventId(), processor));
}