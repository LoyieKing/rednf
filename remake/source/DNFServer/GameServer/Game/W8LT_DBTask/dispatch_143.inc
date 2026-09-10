bool DB_SaveBestClearTime::dispatch(int userIdx, int slot, Stream* stream)
{
    DB_BestClearTimeView* sig = reinterpret_cast<DB_BestClearTimeView*>(
        stream->GetOutBuffer<SIG_BEST_CLEAR_TIME>());
    if (sig == 0)
        return 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    // ORIG：put 返回值先存 bool 局部（mov %al,-0x9）再 movzbl 回读取反判失败
    // （P22 形态；直接 if(!call) 会少缓存两步）。
    bool zipOk = false;
    char* outBuf = 0;
    zipOk = put_compressed_blob_data(
        db, 0, (char*)((unsigned long)sig + 8UL), 0x5dc0, &outBuf);
    if (!zipOk)
    {
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__,
            0xb28d,
            "DB_SaveBestClearTime::dispatch compress_zip(%u) ERROR charac_no=%u",
            0, sig->m_characNo);
        return 0;
    }
    db->set_query("upDate charac_option set best_clear_time = '%s' where charac_no=%u",
                  outBuf, sig->m_characNo);
    if (db->exec(true) != 1)
    {
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__,
            0xb299,
            "DB_SaveBestClearTime->upDate Query ERROR charac_no=%u",
            sig->m_characNo);
        return 0;
    }
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query("inSert into charac_option(charac_no, best_clear_time) values(%u, '%s')",
                      sig->m_characNo, outBuf);
        if (db->exec(true) != 1)
        {
            LogManager::logFormat(
                1, "DBThread.cpp",
                __PRETTY_FUNCTION__,
                0xb2a6,
                "DB_SaveBestClearTime->inSert Query ERROR charac_no=%u",
                sig->m_characNo);
            return 0;
        }
    }
    return 1;
