void CUser::collectPcRoomPlayTime()
{
    // 报告：docs/class_func_reports/CUser/collectPcRoomPlayTime.md。ORIG 0x0868f7ba。
    // ORIG 0x0868f7cd 槽 -0xc(%ebp)；0x0868f7f6 尾赋值 this 先载（uint=uint 同型，技巧5延伸）
    unsigned int now = GlobalData::s_systemTime_.getCurSec();
    // ORIG 0x0868f7d0-0x0868f7ed：total→edx、start→eax、now→ecx（全 uint 同型，无转换）
    m_totalPcRoomPlayTime += now - m_pcRoomPlayStartTime;             // +0x8eba0 / +0x8eb9c
    m_pcRoomPlayStartTime = now;
