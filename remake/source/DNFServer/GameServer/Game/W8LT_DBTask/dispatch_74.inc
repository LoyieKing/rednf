bool DB_SaveAccountInfo::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    DB_SaveAccountInfoView* sig =
        (DB_SaveAccountInfoView*)stream->GetOutBuffer<SIG_SAVE_ACCOUNT_INFO>();
    db->set_query(
        "upDate member_play_info set ip=\"%s\", last_play_time=%u, pcbang_flag=%d, end_ip=\"%s\", mac_addr=\"%s\" where occ_date=now() and m_id=%s",
        sig->m_ip, (unsigned int)sig->m_lastPlayTime, (int)sig->m_pcbangFlag,
        sig->m_endIp, sig->m_macAddr, NumberToString(sig->m_mId, 0));
    // 直判形产生 ORIG 的 mov$1/jmp/mov$0/test al 岛（同 UpdateLogin）。
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into member_play_info(occ_date, m_id, play_count, ip, last_play_time, pcbang_flag, end_ip, mac_addr) values(now(), %s, 1, \"%s\", %u, %d, \"%s\", \"%s\")",
            NumberToString(sig->m_mId, 0), sig->m_ip,
            (unsigned int)sig->m_lastPlayTime, (int)sig->m_pcbangFlag,
            sig->m_endIp, sig->m_macAddr);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
