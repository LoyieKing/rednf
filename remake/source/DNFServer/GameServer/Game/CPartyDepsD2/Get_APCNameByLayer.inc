// ---------------------------------------------------------------------------
// TowerOfDespairMgr::Get_APCNameByLayer —— ORIG T 0x08643e4a：
//   m_apcMgr 非空时转 Get_APC_Name_byIndex。
// ---------------------------------------------------------------------------
const char* TowerOfDespairMgr::Get_APCNameByLayer(const TOD_Layer& layer)
{
    // ORIG 0x08643e4a：非空正分支 inline，null 冷岛在尾。
    if (m_apcMgr != 0)
    {
        return m_apcMgr->Get_APC_Name_byIndex(layer);
    }
    return 0;
}