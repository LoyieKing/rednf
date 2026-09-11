// ---------------------------------------------------------------------------
// ORIG 0x08117398：SetEventFlag(false) +
// cMyTrace(__PRETTY_FUNCTION__,0x20,0)
// ("CSecretShopEvent Event End!")。
void CSecretShopEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x20, 0);
    tr("CSecretShopEvent Event End!");
}