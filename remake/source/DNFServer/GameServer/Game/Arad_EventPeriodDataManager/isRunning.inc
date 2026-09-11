bool Arad_EventPeriodDataManager::isRunning(ENUM_REPEAT_EVENT_CODE eventCode)
{
    // ORIG isRunning@0x08195686：以 s_systemTime_（0x941f714）的当前 tick 作为
    // time 转发给 isApplied（getCurTickCount@0x081458ac 返回 m_curTickCount）。
    return isApplied(eventCode, GlobalData::s_systemTime_.getCurTickCount());
}