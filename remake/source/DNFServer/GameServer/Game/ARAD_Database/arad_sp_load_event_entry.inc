// ORIG 0x0818c8b8：dnf_event_entry 全量装载到 vector<tagDNFEventEntry>。
// [S5-2026-09-11] §6.5 prett：ORIG 串为 `…arad_sp_load_event_entry(SIG_LOGIN_DATA*,
//   DB_Login::vecDNFEventEntry_t&)`（typedef 拼写）⇒ 第2参由展开型改 typedef 型，
//   串逐字节命中；余 `static ` 前缀同一结构性报告（见 premium 处）。
int LOGIN_QUERY::arad_sp_load_event_entry(
    SIG_LOGIN_DATA* data, DB_Login::vecDNFEventEntry_t& out)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x3a5, "%s", "Database null!!");
        return -1;
    }

    // P0：Ghidra C 直接读 env+0x378（=m_serverEnvir.m_serverGroup）内联为实参，
    //   ORIG 无 serverGroup 局部槽（值在 %edx 直传）。
    // P171 照抄原码：ORIG 0x0818c951 为 `test %al,%al; je 主流程`（无 xor）⇒ 原码把
    //   set_query 返回 true 判为失败（与同 TU arad_sp_convert_pass_pad_info
    //   0x0818c16a 的 `xor $1; test; je` 相反）。可观察行为必须与 ORIG 一致 ⇒ 照抄。
    if (db->set_query(
            "seLect event_id,unix_timestamp(occ_date),server_id,charac_no,"
            "unix_timestamp(obtain_date) from dnf_event_entry where m_id=%d "
            "and server_id in(0,%d)",
            data->m_accId, G_CEnvironment()->m_serverEnvir.m_serverGroup))
    {
        return -1;
    }

    // P0：Ghidra C 中 entry（local_2c）为函数级局部，循环外声明一次复用（免每轮 dtor）
    // b28d-solo-ARAD_Database：ORIG 0x818c96b 起五守卫各带独立 nop 中继岛（P223
    //   多岛形：`if(!c) goto Lk` 标签序=守卫序 + 各岛尾 goto 循环尾），且每守卫为
    //   `call; xor $1,%eax; test %al,%al; jne <岛>`（get_int 返回 bool 裸形，P292）。
    // verdict=AE 目标：P167 11/11 CLEAR；set_query 判定按 ORIG 反向形照抄后
    //   111/111（消 xor 1 条）。
    DB_Login::tagDNFEventEntry entry;
    for (unsigned int row = 0; row < static_cast<unsigned int>(db->get_n_rows());
         ++row)
    {
        // P0：Ghidra C 无 occDate/serverId/characNo/obtainDate 中间局部 ——
        // 5 个 get_int 直接写 tagDNFEventEntry 各字段（+0/+8/+0xc/+0x10/+0x14）
        if (!db->get_int(0, entry.m_eventId)) goto skip1;
        if (!db->get_int(1, entry.m_occDate)) goto skip2;
        if (!db->get_int(2, entry.m_serverId)) goto skip3;
        if (!db->get_int(3, entry.m_characNo)) goto skip4;
        if (!db->get_int(4, entry.m_obtainDate)) goto skip5;
        out.push_back(entry);
        goto skipend;
    skip1: goto skipend;
    skip2: goto skipend;
    skip3: goto skipend;
    skip4: goto skipend;
    skip5:;
    skipend:;
    }
    return 0;
}