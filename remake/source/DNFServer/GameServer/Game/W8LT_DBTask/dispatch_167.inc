bool DB_TOD_Reset_UserAPC::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 2;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    db->set_query("seLect charac_no from charac_tower_despair_apc limit 10");
    if (db->exec(true) != 1)
    {
        // 技巧3：三处 trace 全改临时链式（具名块对象致 4.4 把 trace 槽
        // 外置，ORIG 0x084400c8 区 trace=-0x4c 比 guard=-0x54 近 8B；
        // 先例 DB_LoadPowerWarStatueInfo::dispatch 24→3）。
        cMyTrace(__PRETTY_FUNCTION__, 0x9ea7, 5)("TOD : load apc from DB");
        return 0;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9eab), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    SIG_TOD_RELOADED_USER_APC* sig =
        guard->GetInBuffer<SIG_TOD_RELOADED_USER_APC>();
    memset(sig, 0, 0x758);
    // 行视图：SIG_TOD_RELOADED_USER_APC = _APCInfo[10]（行步长 0xbc =
    // sizeof(_APCInfo)，10×0xbc=0x758 与 memset/ORIG `+i*0xbc` 寻址实证
    // 0x084400c8 一致）。
    static_assert(sizeof(_APCInfo) == 0xbc, "TOD APC row stride drift");
    // 技巧36：删具名 apcs 指针（ORIG 站点 `mov -0x18,%eax`+imul 直算，
    // 无第二槽）。
    int nRows = db->get_n_rows();
    if (nRows != 10)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x9eb5, 5)("TOD : get_n_rows() %d",
                                                 nRows);
        return 0;
    }
    for (int i = 0; i < nRows && i < 10; ++i)
    {
        if (db->fetch() != 1)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x9ebd, 5)("TOD : db->fetch(), int i=%d",
                                                     i);
            return 0;
        }
        db->get_int(0, reinterpret_cast<_APCInfo*>(sig)[i].m_userCharacNo);
    }
    for (int i = 0; i < nRows && i < 10; ++i)
    {
        if (reinterpret_cast<_APCInfo*>(sig)[i].m_userCharacNo != 0)
            GetAPCUserInfo(reinterpret_cast<_APCInfo*>(sig)[i]);
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
