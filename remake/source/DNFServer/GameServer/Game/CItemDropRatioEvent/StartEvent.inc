void CItemDropRatioEvent::StartEvent()
{
    SetEventFlag(true);
    m_dropRate = 1;
    G_CDataManager()->SetItemDropRate(1.0f);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x16, 0);
    tr("Item Drop Event Start!");
