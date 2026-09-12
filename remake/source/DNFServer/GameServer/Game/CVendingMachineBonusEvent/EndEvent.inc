// ---------------------------------------------------------------------------
// ORIG 0x08116e94：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__,0x1c,0)
// （ORIG 局部静态符号 0x08b4ed80 "virtual void CVendingMachineBonusEvent::EndEvent()"）。
// [R11fixE5] ORIG 0x08116ed0 串=0x8b4dccc（raw 逐字节
// 'CVendingMachineBonus Event End!'）。
void CVendingMachineBonusEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1c, 0);
    tr("CVendingMachineBonus Event End!");
}