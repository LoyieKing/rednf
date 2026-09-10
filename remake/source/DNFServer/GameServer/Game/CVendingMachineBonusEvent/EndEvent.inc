void CVendingMachineBonusEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1c, 0);
    tr("CVendingMachineBonusEvent Bonus Event End!");
