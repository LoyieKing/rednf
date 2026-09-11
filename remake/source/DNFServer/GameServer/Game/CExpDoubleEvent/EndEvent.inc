void CExpDoubleEvent::EndEvent()
{
    SetEventFlag(false);
    m_factor = 100;
    cMyTrace tr(__FUNCTION__, 0x21, 0);
    tr("Exp Double Event End!");
}