int Inter_TOD_Reload_APC::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    _APCInfo* infos = reinterpret_cast<_APCInfo*>(data);
    static_cast<TowerOfDespairMgr*>(GlobalData::s_pTowerOfDespairMgr)->ResetAPCInfo();
    for (int i = 0; i <= 9; ++i)
    {
        static_cast<TowerOfDespairMgr*>(GlobalData::s_pTowerOfDespairMgr)->SetAPCInfo(
            (unsigned int)i,
            infos[i]);
    }
    return 0;
