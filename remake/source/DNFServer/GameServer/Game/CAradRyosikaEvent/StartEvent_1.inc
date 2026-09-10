void CAradRyosikaEvent::StartEvent(Word_Param param)
{
    // [b28c-L18] N7 封顶复核（fresh .o 20=20）：唯一残差=虚调用 callee 寄存器互换
    // ——ORIG `mov (%eax),%edx; …; call *%edx` vs 4.4 恒间接 call 目标 %eax
    //（探针 /tmp/l22p/vprobe*.cpp + 本轮复查），勿再耗轮次。
    m_field0c = param.m_duration;
    m_field10 = param.m_field2;
    StartEvent();
