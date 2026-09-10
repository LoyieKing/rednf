bool DB_SaveItemDictionaryFlag::dispatch(int /*userIdx*/, int /*slot*/,
                                         Stream* stream)
{
    SIG_ITEM_DICTIONARY_FLAG* sig =
        stream->GetOutBuffer<SIG_ITEM_DICTIONARY_FLAG>();
    if (sig == 0)
        return 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    // ORIG 0x084454c2：`movb $0x0,-0x9` 预清 + call 后 `mov %al,-0x9;
    // movzbl; xor $1; test; je` ⇒ P198① 具名 bool 两句式（callee 返回 bool）。
    bool ok = false;
    char* blob = 0;
    ok = put_compressed_blob_data(db, 0, reinterpret_cast<char*>(sig->m_data),
                                  5000, &blob);
    if (!ok)
    {
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__,
            0xac56,
            "DB_SaveItemDictionaryFalg::dispatch compress_zip(%u) ERROR charac_no=%u",
            0, sig->m_characNo);
        return 0;
    }
    db->set_query(
        "upDate charac_item_stat set check_flag = '%s' where charac_no=%u",
        blob, sig->m_characNo);
    // ORIG 0x08445584：`xor $1; test %al; je <affected 检查>` 早返形
    // （bool callee 的 ==1 被 4.4 折叠，P292/P101）。
    if (!db->exec(true))
    {
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__,
            0xac5e,
            "DB_SaveItemDictionaryFalg->upDate Query ERROR charac_no=%u",
            sig->m_characNo);
        return 0;
    }
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_item_stat(charac_no, check_flag) values(%u, '%s')",
            sig->m_characNo, blob);
        if (!db->exec(true))
        {
            LogManager::logFormat(
                1, "DBThread.cpp",
                __PRETTY_FUNCTION__,
                0xac67,
                "DB_SaveItemDictionaryFalg->inSert Query ERROR charac_no=%u",
                sig->m_characNo);
            return 0;
        }
    }
    return 1;
