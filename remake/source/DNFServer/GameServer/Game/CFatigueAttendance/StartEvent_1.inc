// ---------------------------------------------------------------------------
// ORIG 0x08117470：param 两 ushort 写入 m_addInfo（+0x04/+0x06）→
// 经 vptr slot2 虚调 StartEvent() →
// cMyTrace(__PRETTY_FUNCTION__,0x1c,0)
// ("CFatigueAttendance(AddInfo:%d,%d) Event Start!", m_info0, m_info1)
// （读取以 movswl 符号扩展，变参 %d 求值序 = ORIG esi/ebx）。
void CFatigueAttendance::StartEvent(Word_Param param)
{
    // [b28c-27] N7 封顶：唯一残差=虚调用目标寄存器（ORIG 用 %edx，4.4 恒 %eax）；
    // 其余为 .o 重定位伪差（字符串立即数 + call 目标，AE 口径归一）。
    m_addInfo.m_info0 = param.m_duration;
    m_addInfo.m_info1 = param.m_field2;
    this->StartEvent();
    cMyTrace(__PRETTY_FUNCTION__, 0x1c, 0)(
        "CFatigueAttendance(AddInfo:%d,%d) Event Start!",
        (short)m_addInfo.m_info0, (short)m_addInfo.m_info1);
}