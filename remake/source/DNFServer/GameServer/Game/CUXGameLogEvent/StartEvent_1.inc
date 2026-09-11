// ---------------------------------------------------------------------------
// ORIG 0x08117734：thunk——经 vtable slot 2（StartEvent()）虚调用。
void CUXGameLogEvent::StartEvent(Word_Param param)
{
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
    StartEvent();
}