bool DB_SavePvPMissionList::dispatch(int /*userIdx*/, int /*slot*/,
                                     Stream* stream)
{
    static const int handleIdx = 3;
    MySQL* db;                                  // -0x10
    SIG_REQUEST_PVP_MISSION_LIST* sig;          // -0xc
    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                           E_SERVER_GROUP_NONE);
    sig = stream->GetOutBuffer<SIG_REQUEST_PVP_MISSION_LIST>();
    // [Lane4] 声明序按 ORIG 代码序：ORIG 0x843f8fa 的 rep stos（zipBuf 清零）位于
    // GetDBHandle/GetOutBuffer 两调用**之后**、zipLen 初始化**之前**（实测 0x843f933
    // 起 rep stos，0x843f949 才 movl $0x198,-0x14）。旧序把 zipBuf 提到函数首 ⇒
    // 我方前面多整整一段 rep stos（gap −4/ndiff 62）。
    char zipBuf[0x198] = { '\0' };              // -0x1ac（ORIG rep stos 清零）
    unsigned int zipLen = 0x198;                // -0x14（ORIG rep stos 之后初始化）
    if (!compress_zip(zipBuf, zipLen, (const char*)sig, 0x14a))
        return 0;
    char* str = db->blob_to_str(0, zipBuf, (int)zipLen);
    db->set_query(
        "upDate fair_pvp_score set pvp_mission_info='%s' where charac_no=%u",
        str, sig->m_characNo);
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        str = db->blob_to_str(0, zipBuf, (int)zipLen);
        db->set_query(
            "inSert into fair_pvp_score(charac_no, pvp_mission_info) values(%u, '%s')",
            sig->m_characNo, str);
        if (!db->exec(true))
            return 0;
    }
    return 1;
