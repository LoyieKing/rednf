bool CAssaultPlace::_IsGiveupPanalty(bool flag)
{
    // ORIG 0x82eacf4：GetRepeatEvent 返回 %eax 直通 vtable 装载（无 event 槽）
    // ⇒ 匿名链式调用。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x20)->IsEventing((CUser*)0))
        return false;
    return m_state >= 2 && m_state <= 4 && !flag;
