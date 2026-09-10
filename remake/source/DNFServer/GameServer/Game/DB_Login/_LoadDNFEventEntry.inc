// ============================================================================
// _LoadDNFEventEntry（ORIG T 0x08411bda）
// ============================================================================

void DB_Login::_LoadDNFEventEntry(unsigned int mid, vecDNFEventEntry_t& out)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    // [hc 收] G_CEnvironment()+0x378 = Server_Envir::m_serverGroup（Ordb dnf_event_entry
    // server_id 字段；与 WarRoom::ClearReward/DB_UpdateLogin 同源 +0x378 访问）。
    // [b28d-solo-DB_Login] P0：ORIG 无 serverId 具名槽（值在 %ebx 跨 NumberToString
    // 保活），内联右值直传；P82 视图成员访问折叠单条 disp（裸 (char*)+off 会多 add）。
    struct EnvView { char m_pad[0x378]; unsigned int m_serverGroup; };
    db->set_query(
        "seLect event_id,unix_timestamp(occ_date),server_id,charac_no,unix_timestamp(obtain_date) from dnf_event_entry where m_id=%s and server_id in(0,%d)",
        NumberToString(mid, 0),
        reinterpret_cast<EnvView*>(G_CEnvironment())->m_serverGroup);
    if (!db->exec(true))
    {
        return;
    }
    // [b28d-solo-DB_Login] ORIG 0x08411c58..0x08411d6b：尾部条件块（seta; test; jne body）
    // + body 首 fetch；5 个 get_int 各带独立 nop 岛落到增量（P223 多岛形）；
    // exec/fetch 失败落 epilogue（return）。
    // P209：ORIG `seta` ⇒ 无符号比较（i 声明 unsigned 使 int→unsigned 提升）。
    for (unsigned int i = 0; i < db->get_n_rows(); ++i)
    {
        if (!db->fetch())
        {
            return;
        }
        tagDNFEventEntry entry;
        if (!db->get_int(0, entry.m_eventId)) continue;
        if (!db->get_int(1, entry.m_occDate)) continue;
        if (!db->get_int(2, entry.m_serverId)) continue;
        if (!db->get_int(3, entry.m_characNo)) continue;
        if (!db->get_int(4, entry.m_obtainDate)) continue;
        out.push_back(entry);
    }
}
