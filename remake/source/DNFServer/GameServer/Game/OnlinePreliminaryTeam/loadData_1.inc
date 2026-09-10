bool COnlinePreliminaryMemberMatchList::loadData(CUser* /*pUser*/, char* pData)
{
    // 签名保持 char*（ORIG mangled …EP5CUserPc）；pData 即 MEMBER_MATCH_LIST 布局。
    SIG_ONLINE_PRELIMINARY_MEMBER_MATCH_LIST* d =
        reinterpret_cast<SIG_ONLINE_PRELIMINARY_MEMBER_MATCH_LIST*>(pData);
    for (int i = 0; d->m_count > i; ++i)
    {
        CharacterInServer key(
            static_cast<int>(static_cast<unsigned short>(
                d->m_entries[i].m_serverId)),
            d->m_entries[i].m_uniqueId);
        m_peerMap[key].m_count = d->m_entries[i].m_playCount;
    }
    return true;
