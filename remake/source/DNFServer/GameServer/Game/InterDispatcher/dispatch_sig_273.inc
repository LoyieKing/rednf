// ORIG 0x084e4e26 T dispatch_sig —— 绝望之塔 APC 信息重载。
// 报告 docs/class_func_reports/Inter_TOD_Reload_APC/dispatch_sig.md 全文转写。
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
}