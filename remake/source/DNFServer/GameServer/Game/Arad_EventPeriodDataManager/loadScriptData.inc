bool Arad_EventPeriodDataManager::loadScriptData()
{
    // ORIG 0x08195650：直接调 importAradEventScript(m_eventPeriodList,
    // m_rewardPeriodList)（+0xc/+0x18 两 vector），无 return（eax 残留）。
    ARAD::SCRIPT::importAradEventScript(m_eventPeriodList, m_rewardPeriodList);
}