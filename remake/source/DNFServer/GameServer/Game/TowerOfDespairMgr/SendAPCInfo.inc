void TowerOfDespairMgr::SendAPCInfo(TOD_Layer const& layer, CUser* user)
{
    // ORIG 0x08644338：尾调用 +0x354 处 APC 管理器（无返回值处理）。
    m_apcMgr->Send_APC_Info(layer, user);
