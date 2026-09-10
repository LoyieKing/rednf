void CMissionList_Charac::getData(char* pData) const
{
    // pData = SIG_REQUEST_PVP_MISSION_LIST blob（与 _saveData/loadData 同一布局；
    // ORIG 0x085e4acd `mov %eax,-0x10(%ebp)` 保留一份本地副本）。
    SIG_REQUEST_PVP_MISSION_LIST* pSig =
        reinterpret_cast<SIG_REQUEST_PVP_MISSION_LIST*>(pData);
    std::memset(pSig, 0, sizeof(SIG_REQUEST_PVP_MISSION_LIST));   // 0x14a
    pSig->m_characNo = m_tail14f.m_characNo;    // this+0x150 → blob+0x00
    for (int kind = 0; kind < 36; ++kind)
    {
        pSig->m_missionList[kind].m_index = m_missionList[kind].m_index;
        pSig->m_missionList[kind].m_kind = m_missionList[kind].m_kind;
        pSig->m_missionList[kind].m_count = m_missionList[kind].m_count;
        pSig->m_missionList[kind].m_streak = m_missionList[kind].m_streak;
    }
    {
        // ORIG 直接调用 BitsetToStr(Ph) 重载写 0x20 字节到 pData+0x124（无 string 中间层）
        BitsetToStr(m_clearedFlags, pSig->m_cleared);
    }
    pSig->m_field144 = m_field148;
    pSig->m_field146 = m_field14a;
    pSig->m_field148 = m_field14c;
