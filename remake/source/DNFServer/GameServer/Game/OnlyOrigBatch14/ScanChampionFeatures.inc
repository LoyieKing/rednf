void ScanChampionFeatures(int count, STAICharacterMapArrangeData& data)
{
    std::string s;
    for (int line = 0; line < count; ++line)
    {
        ScanStr(&s);
        for (int i = 0; i <= 0x24; ++i)
        {
            // [PoolB b28f] 表符号名对齐 ORIG：`_ZL21CHAMPION_FEATURE_LIST`（本地副本）
            // + 全局 `CHAMPION_FEATURE_NAME`（STMapScript.cpp 定义，extern 引用）。
            if (s == CHAMPION_FEATURE_NAME[CHAMPION_FEATURE_LIST[i] & 0xff])
            {
                data.m_championFeatures.push_back(
                    (int)CHAMPION_FEATURE_LIST[i]);
                break;
            }
        }
    }
}
