bool DB_UpdateLogout::dispatch(int userIdx, int slot, Stream* stream)
{
    // [L4-r2 判据⑩ 一次性试验] ORIG ok(r)@-0x21 深于 sig@-0x20（4.1 声明序），
    // 我方 r@-0x1d 浅于 sig@-0x24（4.4 首次使用序）⇒ 试把 r 的声明前置。
    bool r;
    SIG_UPDATE_LOGOUT* sig = stream->GetOutBuffer<SIG_UPDATE_LOGOUT>();
    if (sig == 0)
        return 0;
    // [Lane31W8LTDBTaskView] 视图 cast 反模式：`((View*)sig)->f=v` 使 4.4 走
    // base-first（base 提前入 callee-saved 跨 call）；权威头 SIG_UPDATE_LOGOUT
    // 具名字段直访问复现 ORIG RHS-first（0x0841865c `mov 0x10(%ebp),%edx` …
    // 0x0841866e `mov -0x20(%ebp),%edx; mov %eax,0xe(%edx)`）。
    sig->m_serverId = 0;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        sig->m_serverId = GlobalData::s_db_mgr->GetServerType(slot);
    else
        sig->m_serverId = G_CEnvironment()->m_serverEnvir.m_serverGroup;
    GlobalData::s_db_mgr->removeUserDBInfo(slot);
    r = UpdateLogout(sig);
    if (!r)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x4939,
                              "UpdateLogout() fail - m_id[%s]\n",
                              NumberToString(((DB_UpdateLogoutSigView*)sig)->m_accId, 0));
        return 0;
    }
    if (((DB_UpdateLogoutSigView*)sig)->m_stateFlag)
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
        if (((DB_UpdateLogoutSigView*)sig)->m_pcbang)
        {
            db->set_query("upDate member_play_info set play_time=play_time+%d,play_count=play_count+1,trade_cnt=trade_cnt+%d, exp=exp+%u, used_fatigue=used_fatigue+%u, ting_count=ting_count+%d, ip='%s', last_play_time=%u, pcbang_flag=%d, end_ip='%s', mac_addr='%s', server_id = %d where occ_date=cast(now() as date) and m_id=%s",
                          ((DB_UpdateLogoutSigView*)sig)->m_playTime,
                          ((DB_UpdateLogoutSigView*)sig)->m_field17,
                          ((DB_UpdateLogoutSigView*)sig)->m_field1b,
                          ((DB_UpdateLogoutSigView*)sig)->m_field1f,
                          ((DB_UpdateLogoutSigView*)sig)->m_puUser ? 0 : 1,
                          ((DB_UpdateLogoutSigView*)sig)->m_field37,
                          ((DB_UpdateLogoutSigView*)sig)->m_field23,
                          ((DB_UpdateLogoutSigView*)sig)->m_pcbang,
                          ((DB_UpdateLogoutSigView*)sig)->m_field43,
                          ((DB_UpdateLogoutSigView*)sig)->m_mac,
                          ((DB_UpdateLogoutSigView*)sig)->m_serverId,
                          NumberToString(((DB_UpdateLogoutSigView*)sig)->m_accId, 0));
        }
        else
        {
            db->set_query("upDate member_play_info set play_time=play_time+%d,play_count=play_count+1,trade_cnt=trade_cnt+%d, exp=exp+%u, used_fatigue=used_fatigue+%u, ting_count=ting_count+%d, ip='%s', last_play_time=%u, end_ip='%s', mac_addr='%s', server_id = %d where occ_date=cast(now() as date) and m_id=%s",
                          ((DB_UpdateLogoutSigView*)sig)->m_playTime,
                          ((DB_UpdateLogoutSigView*)sig)->m_field17,
                          ((DB_UpdateLogoutSigView*)sig)->m_field1b,
                          ((DB_UpdateLogoutSigView*)sig)->m_field1f,
                          ((DB_UpdateLogoutSigView*)sig)->m_puUser ? 0 : 1,
                          ((DB_UpdateLogoutSigView*)sig)->m_field37,
                          ((DB_UpdateLogoutSigView*)sig)->m_field23,
                          ((DB_UpdateLogoutSigView*)sig)->m_field43,
                          ((DB_UpdateLogoutSigView*)sig)->m_mac,
                          ((DB_UpdateLogoutSigView*)sig)->m_serverId,
                          NumberToString(((DB_UpdateLogoutSigView*)sig)->m_accId, 0));
        }
        if (((DB_UpdateLogoutSigView*)sig)->m_accId != 0)
        {
            if (!db->exec(true) || db->getAffectedRowCount() == 0)
            {
                db->set_query("inSert into member_play_info(occ_date,m_id,play_time,play_count,trade_cnt,exp,used_fatigue,ting_count, ip, last_play_time, pcbang_flag, end_ip, mac_addr, server_id) values(cast(now() as date),%s,%d,1,%d,%u,%u,%d,'%s', %u, %d, '%s', '%s',%d)",
                              NumberToString(((DB_UpdateLogoutSigView*)sig)->m_accId, 0),
                              ((DB_UpdateLogoutSigView*)sig)->m_playTime,
                              ((DB_UpdateLogoutSigView*)sig)->m_field17,
                              ((DB_UpdateLogoutSigView*)sig)->m_field1b,
                              ((DB_UpdateLogoutSigView*)sig)->m_field1f,
                              ((DB_UpdateLogoutSigView*)sig)->m_puUser ? 0 : 1,
                              ((DB_UpdateLogoutSigView*)sig)->m_field37,
                              ((DB_UpdateLogoutSigView*)sig)->m_field23,
                              ((DB_UpdateLogoutSigView*)sig)->m_pcbang,
                              ((DB_UpdateLogoutSigView*)sig)->m_field43,
                              ((DB_UpdateLogoutSigView*)sig)->m_mac,
                              ((DB_UpdateLogoutSigView*)sig)->m_serverId);
                if (!db->exec(true))
                    return 0;
            }
        }
    }
    if (((DB_UpdateLogoutSigView*)sig)->m_field88)
        UpdateAccountMemberBonusFatigue(((DB_UpdateLogoutSigView*)sig)->m_accId,
                                        ((DB_UpdateLogoutSigView*)sig)->m_field89);
    return SendResult(userIdx, slot, sig);
