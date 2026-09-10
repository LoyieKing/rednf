void CVendingMachineBonusEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x12, 0);
    tr("CVendingMachineBonusEvent Bonus Event Start!");
