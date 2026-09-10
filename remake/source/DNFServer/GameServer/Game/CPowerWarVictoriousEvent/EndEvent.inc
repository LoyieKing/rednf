void CPowerWarVictoriousEvent::EndEvent()
{
    // [N7 封顶] IsEventing 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换。
    if (IsEventing(0))
    {
        SetEventFlag(false);
        m_param.m_duration = 0;
        m_param.m_field2 = 0;
        cMyTrace tr(__PRETTY_FUNCTION__, 0x41, 0);
        tr("Power War Victorious Event End");
    }
}
