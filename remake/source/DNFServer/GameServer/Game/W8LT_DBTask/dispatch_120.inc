// ============================================================================
// DB_RemoveEventItem::dispatch —— ORIG 0x0841ab30（报告 dispatch.md）。
// GetDBHandle(DB_HANDLE_3,0) → GetOutBuffer<SIG_REMOVE_EVENTITEM> →
// 循环每条 upDate charac_event_items set delete_flag=1,delete_time=now()
// where id=%u → exec(true)（结果不检查）；恒返 1。
// ============================================================================
bool DB_RemoveEventItem::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    // view 类型局部（ORIG 循环体「先载下标再载基址」形态；证据 dispatch.md
    // 0x841ab6d：mov -0xc,%edx / mov -0x10,%eax / mov 0x4(%eax,%edx,4),%eax）。
    DB_RemoveEventItemView* raw =
        (DB_RemoveEventItemView*)stream->GetOutBuffer<SIG_REMOVE_EVENTITEM>();
    int i;
    for (i = 0; i < raw->m_count; ++i)
    {
        db->set_query(
            "upDate charac_event_items set delete_flag=1,delete_time=now() where id=%u",
            raw->m_ids[i]);
        db->exec(true);
    }
    return 1;
}