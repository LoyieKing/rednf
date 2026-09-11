int Inter_LoadBloodDungeonBestRecord::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0x3edd;
    // ORIG：包基址槽在 get_state 守卫之后物化。
    Inter_LoadBloodDungeonBestRecordPktView* pPkt = reinterpret_cast<Inter_LoadBloodDungeonBestRecordPktView*>(data);
    const int idx = user->get_charac_idx(pPkt->m_0);
    if (idx < 0)
        return 0x3ee3;
    // ORIG +0x3e 一步 `mov (%eax),%edx`（同 signedness 折叠）⇒ 不加显式转换。
    if (user->m_characList[idx].m_characNo !=
        pPkt->m_0)
        return 0x3ee5;
    user->m_characList[idx].m_characNo =
        pPkt->m_0;
    user->setBloodBestRecord(
        pPkt->m_4,
        pPkt->m_8,
        pPkt->m_c, true);
    return 0;
}