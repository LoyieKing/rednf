void CWeekendBonusEvent::StartEvent(Word_Param)  // ORIG 0x0811760c：虚调用 slot2（StartEvent()）
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
{
    StartEvent();
}