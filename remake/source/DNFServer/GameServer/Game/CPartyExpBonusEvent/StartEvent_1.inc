// ORIG T 0x08116cd0：m_field0c = param.m_duration 后转调无参 StartEvent()
//（经 vptr+0x08 槽虚调用）。
void CPartyExpBonusEvent::StartEvent(Word_Param param)
{
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
    m_field0c = (unsigned int)param.m_duration;
    StartEvent();
}