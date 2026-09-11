void CBattle_Field::CBloodClearRewardData::incUltimateRoundClearPoint(int v)
{
    // ORIG 08307096：m_9c8 += dm_intTable[v+0x185c]；视图 m_arr[v] 折算同址。
    m_ultimateClearPoint =
        m_ultimateClearPoint
        + ((CDataManager_UltimateArr_proxy*)G_CDataManager())->m_arr[v];
}