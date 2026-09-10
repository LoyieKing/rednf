bool DB_BlackIPMonitorPartLoad::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    struct tm* ptm = localtime((const time_t*)&GlobalData::g_IPMonitorLastDBAccessTime);
    char timeBuf[0x14] = { '\0' };
    sprintf(timeBuf, "%d-%d-%d %d:%d:%d", ptm->tm_year + 0x76c, ptm->tm_mon + 1,
            ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    db->set_query(
        "seLect ip, type, m_id_cnt, unix_timestamp(start_time), unix_timestamp(end_time) from ip_monitor_punish where start_time > '%s'",
        timeBuf);
    if (!db->exec(true) || db->get_n_rows() == 0)
        return 0;
    // P227 全表达式临时链：ORIG 0x084331xx 在 ctor 前 `call get_n_rows;
    // mov %eax,%ebx` 并把 %ebx 跨 ctor 保活到 op() 实参（具名对象时 4.4 在
    // ctor 之后重调 get_n_rows，多一次 DB 调用）。
    cMyTrace(__PRETTY_FUNCTION__, 0x7ac4, 0)(
        "BLACKIP GET DB ROWS COUNT : %d", db->get_n_rows());
    char buf[0xc80];
    // [A04-r5i · 轴 B/P196] ORIG 0x084333cc `cmpl $0x62,-0x24(%ebp); **ja**`、
    // 0x084333dc `cmp -0x28(%ebp),%eax; **seta**`、0x08433503 循环尾 `**seta** %al;
    // test; jne 0x08433255` ⇒ 三处**全为无符号序** ⇒ 按 ORIG 把 rowCount/i 声明为
    // unsigned（原 int ⇒ 4.4 产 `jg/jle/setg`，与 ORIG 相反）；P196 前置通过（向 ORIG 对齐）。
    unsigned int rowCount = 0;
    for (unsigned int i = 0; i < db->get_n_rows(); ++i)
    {
        if (!db->fetch())
            return 0;
        if (rowCount == 0)
            memset(buf, 0, 0xc80);
        // P183：ORIG 每处 `lea -0xcdc(%ebp),%eax; shl $0x5,%edx; add %edx,%eax`
        // 重算 buf+rowCount*0x20；具名 items 指针缓存会省掉该链。
        if (!db->get_str(0, ((DB_IpMonitorPunishItem*)buf)[rowCount].m_ip, 0x10))
            return 0;
        if (!db->get_int(1, ((DB_IpMonitorPunishItem*)buf)[rowCount].m_type))
            return 0;
        if (!db->get_uint(2, ((DB_IpMonitorPunishItem*)buf)[rowCount].m_idCnt))
            return 0;
        if (!db->get_uint(3, ((DB_IpMonitorPunishItem*)buf)[rowCount].m_startTime))
            return 0;
        if (!db->get_uint(4, ((DB_IpMonitorPunishItem*)buf)[rowCount].m_endTime))
            return 0;
        if (rowCount <= 0x62 && i < db->get_n_rows() - 1)
        {
            ++rowCount;
        }
        else
        {
            CStreamGuard guard(
                GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7ae9), true);
            guard.operator*()->operator<<((int)0x156);
            guard.operator*()->operator<<((int)-1);
            // [Lane31W8LTDBTaskView] 同上（ORIG 0x084330d4 段 RHS-first）。
            SIG_IP_MONITOR_PUNISH* view =
                guard->GetInBuffer<SIG_IP_MONITOR_PUNISH>();
            view->m_count = rowCount + 1;
            memcpy(view->m_items, buf, 0xc80);
            view->m_flag = 0;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            rowCount = 0;
        }
    }
    return 1;
