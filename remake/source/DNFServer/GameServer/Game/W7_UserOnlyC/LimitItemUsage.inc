LimitItemUsage::LimitItemUsage()
{
    reinterpret_cast<CommonTime*>(this)->SetCurTime();   // P134 内联实参
    m_field10 = 0;
    // fresh ORIG C1 为 movl $0,0x11（int 宽清零；旧"四 movb"注记作废，P196）。
    m_field11 = 0;
    m_field15 = 0;
    m_field19 = 0;
