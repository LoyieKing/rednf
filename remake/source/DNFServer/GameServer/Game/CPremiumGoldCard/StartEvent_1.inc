// ---------------------------------------------------------------------------
// ORIG 0x0811c4ee：vtable+0x8 直调 StartEvent()（转发无参版本）。
void CPremiumGoldCard::StartEvent(Word_Param param)
{
    // [b28c-27] N7 封顶：唯一残差=虚调用目标寄存器（ORIG 用 %edx，4.4 恒 %eax）。
    StartEvent();
}