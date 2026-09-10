void TowerOfDespairMgr::SetAPCInfo(unsigned int idx, _APCInfo const& info)
{
    TOD_Layer layer(m_npcLayers[idx]);
    m_apcMgr->Set_APC_Info(layer, info);
