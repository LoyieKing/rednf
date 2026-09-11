bool DB_SelectLevelUpReward::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 2;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    unsigned int characNo = 0;
    int rewardIdx = 0;
    *stream >> characNo;
    *stream >> rewardIdx;
    std::string tableStr(
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())->get_db_table_name(0x194));

    db->set_query("seLect  recv_level from %s where charac_no = %u",
                  toTString(tableStr), characNo);
    if (!db->exec(true))
        return 0;

    int rows = db->get_n_rows();
    unsigned char recvLevel = 0;
    if (rows != 0)
    {
        if (!db->fetch())
            return 0;
        if (!db->get_ubyte(0, recvLevel))
            return 0;
        if (recvLevel == rewardIdx)
        {
            LogManager::logFormat(
                1, "DBThread.cpp",
                __PRETTY_FUNCTION__,
                0xa2a5,
                "DB_SelectLevelUpReward error char:%u lv:%u recv_lv:%u",
                characNo, rewardIdx, (unsigned int)recvLevel);
            return 0;
        }
        db->set_query(
            "upDate %s set recv_level=%u,occ_time=now() where charac_no=%u",
            toTString(tableStr), rewardIdx, characNo);
        if (!db->exec(true))
            return 0;
    }
    else
    {
        db->set_query(
            "inSert into %s(charac_no,recv_level,occ_time) values (%u,%u,now() )",
            // ORIG 0x84425b3-0x84425c8：`mov -0x18,%esi; mov -0x14,%ebx; …
            // mov %esi,0x10(%esp); mov %ebx,0xc(%esp)`（-0x14=characNo→arg2，
            // -0x18=rewardIdx→arg3）—— 列序 (charac_no,recv_level) 对应实参
            // (characNo, rewardIdx)；此前写反属语义 bug。
            toTString(tableStr), characNo, rewardIdx);
        if (!db->exec(true))
            return 0;
    }

    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa2ba), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    guard.operator*()->operator<<(rewardIdx);
    guard.operator*()->operator<<((int)recvLevel);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}