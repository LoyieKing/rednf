// ============================================================================
// ORIG 0x081666e6（StartEvent_081666e6.md 全文已读）：thunk——经 vptr slot 2
// 虚调用 StartEvent()。镜像布局无 vtable，直接调用等价。
// ============================================================================
void HeroMissionEvent::StartEvent(Word_Param param)
{
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
    StartEvent();
}