// ---------------------------------------------------------------------------
// ORIG 0x0811c3e0：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__,0x1a,0)
// ("CDoubleGoldCardEvent Event End!")。
void CDoubleGoldCardEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1a, 0);
    tr("CDoubleGoldCardEvent Event End!");
}