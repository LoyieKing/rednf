void CItemDropRatioEvent::EndEvent()
{
    SetEventFlag(false);
    m_dropRate = 1;
    G_CDataManager()->SetItemDropRate(1.0f);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x28, 0);
    tr("Item Drop Event End!");
