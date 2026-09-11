// ---------------------------------------------------------------------------
// ORIG T 0x82ebd64 (size 0x41)：_IsProcsssChaos() —— 事件未进行时 true
// （GetRepeatEvent(0x20)→vtable+0x34 槽 IsEventing；!=1 时返回 true）
// ---------------------------------------------------------------------------
bool CAssaultPlace::_IsProcsssChaos()
{
    if (GlobalData::s_event_manager->GetRepeatEvent(0x20)->IsEventing((CUser*)0))
        return false;
    return true;
}