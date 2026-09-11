// TowerOfDespairMgr::SetAPCInfo —— ORIG T 0x086442ca [0x086442ca,0x0864431d]
// vector<unsigned int>::operator[](m_npcLayers, idx) 构造 TOD_Layer，
// 转发 +0x354 APC 管理器 Set_APC_Info。
void TowerOfDespairMgr::SetAPCInfo(unsigned int idx, _APCInfo const& info)
{
    TOD_Layer layer(m_npcLayers[idx]);
    m_apcMgr->Set_APC_Info(layer, info);
}