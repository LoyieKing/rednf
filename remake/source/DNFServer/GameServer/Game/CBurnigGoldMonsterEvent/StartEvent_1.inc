// ORIG 0x08116fa2（thunk，0x1a）：经 vptr+8 虚调 StartEvent()。
// [b28c-30] N7 封顶：ORIG 间接 call 目标 %edx（`mov (%eax),%edx; …; call *%edx`）
// vs 4.4 恒 %eax，助记符全等仅寄存器名差（同 CPCRoomWorldDropEvent 件）。
void CBurnigGoldMonsterEvent::StartEvent(Word_Param /*param*/)
{
    StartEvent();
}