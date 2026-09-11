// ---------------------------------------------------------------------------
// ORIG 0x08116e28：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0x12,0)
// （ORIG 局部静态符号 0x08b4edc0 "virtual void CVendingMachineBonusEvent::StartEvent()"，
//  日志串 0x08b4dca8 "CVendingMachineBonusEvent Bonus Event Start!"）。
void CVendingMachineBonusEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x12, 0);
    tr("CVendingMachineBonusEvent Bonus Event Start!");
}