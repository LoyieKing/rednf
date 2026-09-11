// ---------------------------------------------------------------------------
// ORIG 0x08267910：vtable+0x8 直调 StartEvent()（转发无参版本）。
// [b28c-30] N7 封顶：ORIG 取槽后 `mov (%eax),%edx; mov 0x8(%ebp),%eax;
// mov %eax,(%esp); call *%edx`（4.1 间接目标 %edx）；4.4 恒目标 %eax
// （`mov (%eax),%eax; …; call *%eax`）。助记符序列全等仅寄存器名差。
void CPCRoomWorldDropEvent::StartEvent(Word_Param param)
{
    StartEvent();
}