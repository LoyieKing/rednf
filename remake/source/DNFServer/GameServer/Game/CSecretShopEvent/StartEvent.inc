// ---------------------------------------------------------------------------
// ORIG 0x0811732c：SetEventFlag(true) +
// cMyTrace(__PRETTY_FUNCTION__,0x16,0)
// ("CSecretShopEvent Event Start!")。
void CSecretShopEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x16, 0);
    tr("CSecretShopEvent Event Start!");
}