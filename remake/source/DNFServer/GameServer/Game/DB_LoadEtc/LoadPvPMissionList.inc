bool DB_LoadEtc::LoadPvPMissionList(SIG_LOAD_ETC* sig)
{
    static const int handleIdx = 3;    // ORIG _ZZN10DB_LoadEtc18LoadPvPMissionList…E9handleIdx
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect pvp_mission_info from fair_pvp_score where charac_no=%u",
                  sig->m_characNo);
    bool ok = db->exec(true);                                // ORIG -0xd 具名槽
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b3f, 5)(
            "LoadPvPMissionList, exec() ERROR charac_no=%u", sig->m_characNo);
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return false;
    }
    ok = db->fetch();
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b49, 5)(
            "LoadPvPMissionList, fetch() ERROR charac_no=%u", sig->m_characNo);
        return false;
    }

    // ORIG 0x08406d…：`char blob[0x198]={0}` 内联 rep stos（P214③，非 memset call）；
    // outLen 具名槽写后读回（P244：get_binary 第 3 参复用 outLen 槽，非直填 0x198）。
    char blob[0x198] = { 0 };
    unsigned int outLen;
    outLen = 0x198;
    if (!db->get_binary(0, blob, outLen))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b51, 5)(
            "DB_LoadInventory::GetItemLock, get_binary(2) ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }
    unsigned int srcLen = db->get_binary_length(0);
    if (!uncompress_zip(sig->m_pvpMissionInfo, outLen, blob, srcLen))
    {
        return false;
    }
    return true;
