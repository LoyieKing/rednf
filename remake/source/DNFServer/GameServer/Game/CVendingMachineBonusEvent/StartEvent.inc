// ---------------------------------------------------------------------------
// ORIG 0x08116e28：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0x12,0)
// （ORIG 局部静态符号 0x08b4edc0 "virtual void CVendingMachineBonusEvent::StartEvent()"）。
// [R11fixE5] ORIG 0x08116e64 串=0x8b4dca8（raw 逐字节
// 'CVendingMachineBonus Event Start!'，无第二个 "Bonus"/"Event" 后缀）。
void CVendingMachineBonusEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x12, 0);
    tr("CVendingMachineBonus Event Start!");
}