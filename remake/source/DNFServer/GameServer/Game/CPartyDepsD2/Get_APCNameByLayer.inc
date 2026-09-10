const char* TowerOfDespairMgr::Get_APCNameByLayer(const TOD_Layer& layer)
{
    // ORIG 0x08643e4a：非空正分支 inline，null 冷岛在尾。
    if (m_apcMgr != 0)
    {
        return m_apcMgr->Get_APC_Name_byIndex(layer);
    }
    return 0;
