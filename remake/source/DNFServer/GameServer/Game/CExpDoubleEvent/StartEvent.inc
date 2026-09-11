void CExpDoubleEvent::StartEvent()
{
    SetEventFlag(true);
    m_factor = 100;
    cMyTrace tr(__FUNCTION__, 0x13, 0);
    tr("Exp Double Event Start!");
}