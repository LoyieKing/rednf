const char* TowerOfDespair_APC_Mgr::Get_APC_Name_byIndex(const TOD_Layer& layer)
{
    std::map<TOD_Layer, SendingAPC_Info>::iterator it;   // P50 复活：独立 C1Ev
    std::map<TOD_Layer, SendingAPC_Info>::iterator e = m_APCInfoMap.end();
    it = m_APCInfoMap.find(layer);
    if (it != e)
        return it->second.get_user_charac_name();
    return 0;
