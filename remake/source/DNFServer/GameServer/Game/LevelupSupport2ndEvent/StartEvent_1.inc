// ---------------------------------------------------------------------------
// ORIG 0x0811ba44：thunk——经 vtable slot 2（StartEvent()）虚调用。
// [b28c-30] N7 封顶：同 CPCRoomWorldDropEvent::StartEvent(Word_Param)——
// ORIG 间接 call 目标 %edx vs 4.4 恒 %eax，助记符全等仅寄存器名差。
void LevelupSupport2ndEvent::StartEvent(Word_Param param)
{
    StartEvent();
}