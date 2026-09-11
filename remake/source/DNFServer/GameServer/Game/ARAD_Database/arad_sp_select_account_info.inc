// ORIG 0x0818b706：member_info 主账号信息。逐列解析失败仅 trace 并返 0（ORIG 如此）；
//   成功时写 passwd 数值(+0xc0)、regdate(+0xcc)、hangame 标志(+0x38dd)、
   // ssn 拼接(+0x2284)与出生年(+0xc8)。【ABI 镜像】字段见 login_field 标注偏移。
// verdict=DIFF 353/356（gap=-3）：P167 调用点集 34/34 CLEAR。已修：
//   ①删 `unsigned int year` 具名槽（ORIG 两臂各自直写 data->m_birthYear，P0）；
//   ②10 处具名 cMyTrace 对象改**全表达式临时链** `cMyTrace(f,l,v)(fmt,…)`（P227，
//     Main/L4 实证配方）⇒ ORIG prologue `push %ebx; sub $0xf4` 逐条复现
//     （旧具名形 4.4 无 push %ebx、帧 0xf8），difflib 差异段 72→27。
// 残余（语义等价）：①col 清零与 -0xc0 槽的先后（ORIG -0xc 先）；②成员取址
//   `lea 0xc0(%eax),%edx` vs 4.4 `add $0xc0,%eax`（N107 六形探针全败）；③实参装载序
//   （P75，ORIG 先读 col 再压常量/参数）⇒ 净 3 条。语义闭合。
// [S5-2026-09-11 复攻] gap −3→**0**（356→353，ndiff 75→66）：①删 snprintf 两处
//   `static_cast<int>`（直接传 unsigned，变参提升等价）⇒ 消 1 条 mov；②出生年两臂
//   `(int)fssn + 2000/1900`（int 域加法）⇒ 4.4 由 `add $imm,%eax; mov %eax,%edx`
//   翻成 ORIG `lea imm(%eax),%edx`，消 2 条 mov（重定基址）。负结论：`++col` 拆成
//   `++col; get_uint(col,…)` 实测零变化（4.4 仍先算 arg3 再读 col）⇒ 已回滚；
//   余下 3 处 lea↔add 为纯 N107（4.1 lea/4.4 add 风格差），源码不可择。
int LOGIN_QUERY::arad_sp_select_account_info(SIG_LOGIN_DATA* data, char* param_2)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x1e8, "%s", "Database null!!");
        return -1;
    }

    if (db->select(
            "seLect m_id, passwd,first_ssn, second_ssn, "
            "mid(from_unixtime(reg_date)+0,1,8),user_name,email, user_id, hangame_flag "
            "from member_info where m_id=%d",
            data->m_accId) != 0)
    {
        return -1;
    }
    if (db->get_n_rows() != 1)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1f5, 5)("query get_n_rows() ERROR m_id=%u", data->m_accId);
        return 0;
    }

    unsigned int fssn = 0;                    // first_ssn
    unsigned int sssn = 0;                    // second_ssn
    unsigned int regMid = 0;                  // mid(from_unixtime(reg_date))
    unsigned int hangameFlag = 0;
    // ORIG 0x818b7f0 起 col 槽 -0xc 为 0，各调用实参为 `addl $1,槽; mov 槽,%eax`
    //   ⇒ 源形统一 `++col`（先自增后读）。
    int col = 0;
    if (!db->get_uint(++col, data->m_accId))       // col1
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x201, 5)("user m_id ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_str(++col, param_2, 0x84))                              // col2
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x207, 5)("user password ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_uint(++col, fssn))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x20d, 5)("user fssn ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_uint(++col, sssn))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x213, 5)("user sssn ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_uint(++col, regMid))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x219, 5)("user regdate ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_str(++col, data->m_userDetailIpPort, 0x80))  // user_name（+0x2184）
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x21f, 5)("user user_name ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_str(++col, data->m_userDetailSsn, 0x80))  // email（+0x2204）
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x225, 5)("user email ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_str(++col, reinterpret_cast<char*>(data), 0x1e))           // user_id
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x22b, 5)("user id ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_uint(++col, hangameFlag))                                  // col9
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x231, 5)("user HanGameUser Check ERROR m_id=%u", data->m_accId);
        return 0;
    }

    data->m_regDate = regMid;
    data->m_hangameUser =
        static_cast<unsigned char>(hangameFlag == 1 ? 1 : 0);
    OS_API::snprintf(data->m_birth, 0x10, "%d%d",   // +0x2284
                     fssn, sssn);
    // ORIG 0x0818bcf1：先 fssn/=10000、sssn/=1000000（mul/shr 常量除法），
    // 再 sssn > 2 → year=fssn+2000(0x7d0)，否则 year=fssn+1900(0x76c)。
    fssn = fssn / 10000;
    sssn = sssn / 1000000;
    if (sssn > 2)
    {
        data->m_birthYear = static_cast<int>(fssn) + 2000;
    }
    else
    {
        data->m_birthYear = static_cast<int>(fssn) + 1900;
    }
    return 0;
}