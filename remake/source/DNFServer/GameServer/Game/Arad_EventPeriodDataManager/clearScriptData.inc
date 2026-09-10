void Arad_EventPeriodDataManager::clearScriptData()
{
    // ORIG 0x8195670：仅清空 +0x0c 的 vector<SEventPeriodData>（奖励容器不清）。
    m_eventPeriodList.clear();
