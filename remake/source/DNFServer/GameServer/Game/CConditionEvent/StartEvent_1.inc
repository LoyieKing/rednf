// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ORIG 0x0818490e：thunk——经 vtable slot 2（StartEvent()）虚调用。
void CConditionEvent::StartEvent(Word_Param param)
{
    // N7 封顶：唯一残差=ORIG 虚调用 fn→%edx/this→%eax vs 4.4 fn→%eax/this→%edx
    // （探针 /tmp/l22p/vprobe*.cpp 7 源形全 eax，不可翻；ndiff 8）。
    StartEvent();
}