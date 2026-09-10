bool Arad_EventPeriodDataManager::isApplied(ENUM_REPEAT_EVENT_CODE eventCode, unsigned int time)
{
    // P167：ORIG 用 SearchEventPeriod 谓词 + find_if（非 const iterator）
    SearchEventPeriod pred(eventCode);
    std::vector<ARAD::SCRIPT::SEventPeriodData>::iterator it =
        std::find_if(m_eventPeriodList.begin(), m_eventPeriodList.end(), pred);
    if (!(it != m_eventPeriodList.end()))
        return false;
    return (it->m_beginTime <= time) && (time < it->m_endTime);
