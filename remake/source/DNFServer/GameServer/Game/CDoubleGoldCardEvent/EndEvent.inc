void CDoubleGoldCardEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1a, 0);
    tr("CDoubleGoldCardEvent Event End!");
