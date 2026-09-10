void CSecretShopEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x20, 0);
    tr("CSecretShopEvent Event End!");
