int Inter_LoadBloodDungeonBestRecord::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0x3edd;
    // ORIG：包基址槽在 get_state 守卫之后物化。
    SIG_LOAD_BLOOD_DUNGEON_BEST_RECORD* rec = reinterpret_cast<SIG_LOAD_BLOOD_DUNGEON_BEST_RECORD*>(data);
    const int idx = user->get_charac_idx(rec->m_characNo);
    if (idx < 0)
        return 0x3ee3;
    // ORIG +0x3e 一步 `mov (%eax),%edx`（同 signedness 折叠）⇒ 不加显式转换。
    if (user->m_characList[idx].m_characNo !=
        rec->m_characNo)
        return 0x3ee5;
    user->m_characList[idx].m_characNo =
        rec->m_characNo;
    user->setBloodBestRecord(
        rec->m_bestTime,
        rec->m_bestRound,
        rec->m_dungeonIndex, true);
    return 0;
}