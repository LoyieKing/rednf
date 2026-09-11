// ---------------------------------------------------------------------------
// ORIG 0x0811c374：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0x10,0)
// ("CDoubleGoldCardEvent Event Start!")。
void CDoubleGoldCardEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x10, 0);
    tr("CDoubleGoldCardEvent Event Start!");
}