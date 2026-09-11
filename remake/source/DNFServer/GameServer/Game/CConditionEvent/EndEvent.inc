// ---------------------------------------------------------------------------
// ORIG 0x08184928：logFormat(1,"localjapan/Arad_ConditionEvent.cpp",
// __PRETTY_FUNCTION__,0x17,"[CConditionEvent] End Event.") 后 SetEventFlag(false)。
void CConditionEvent::EndEvent()
{
    LogManager::logFormat(1, "localjapan/Arad_ConditionEvent.cpp",
        __PRETTY_FUNCTION__, 0x17, "[CConditionEvent] End Event.");
    SetEventFlag(false);
}