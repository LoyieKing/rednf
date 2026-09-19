bool Arad_EventPeriodDataManager::loadScriptData()
{
    // ORIG 0x08195650：直接调 importAradEventScript(m_eventPeriodList,
    // m_rewardPeriodList)（+0xc/+0x18 两 vector），无 return（eax 残留）。
    ARAD::SCRIPT::importAradEventScript(m_eventPeriodList, m_rewardPeriodList);
#if !remake_verify
    return 1;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}