// TowerOfDespairMgr::ResetAPCInfo —— ORIG T 0x0864431e [0x0864431e,0x08644337]
// 转发 +0x354 APC 管理器 Reset_APC_Info（mov 0x354(%eax),%eax; call）。
void TowerOfDespairMgr::ResetAPCInfo()
{
    m_apcMgr->Reset_APC_Info();
}