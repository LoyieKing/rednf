void CDoubleGoldCardEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x10, 0);
    tr("CDoubleGoldCardEvent Event Start!");
