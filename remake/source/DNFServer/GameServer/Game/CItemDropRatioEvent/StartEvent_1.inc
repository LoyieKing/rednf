void CItemDropRatioEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    m_dropRate = param.m_duration;
    G_CDataManager()->SetItemDropRate((float)m_dropRate);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1f, 0);
    tr("Item Drop Event Start!");
