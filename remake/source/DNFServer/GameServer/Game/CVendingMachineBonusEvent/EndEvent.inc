// ---------------------------------------------------------------------------
// ORIG 0x08116e94：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__,0x1c,0)
// （ORIG 局部静态符号 0x08b4ed80 "virtual void CVendingMachineBonusEvent::EndEvent()"，
//  日志串 0x08b4dccc "CVendingMachineBonusEvent Bonus Event End!"）。
void CVendingMachineBonusEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1c, 0);
    tr("CVendingMachineBonusEvent Bonus Event End!");
}