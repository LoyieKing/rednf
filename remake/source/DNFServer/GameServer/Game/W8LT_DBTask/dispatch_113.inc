bool DB_GetUserRegdate::dispatch(int userIdx, int slot, Stream* stream)
{
    unsigned int day = 0;   // -0x14 DATE_FORMAT 列
    unsigned int reg = 0;   // -0x18 reg_date 列
    SIG_GET_USER_REGDATE* sig =
        stream->GetOutBuffer<SIG_GET_USER_REGDATE>();  // -0x10
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect DATE_FORMAT(from_unixtime(reg_date),'%%Y%%m%%d%%H') ,reg_date from member_info where m_id=%d",
        sig->m_id);
    if (!db->exec(true))
        return 0;
    if (db->get_n_rows() != 0)
    {
        if (!db->fetch())
            return 0;
        if (!db->get_uint(0, day))
            return 0;
        if (!db->get_uint(1, reg))
            return 0;
    }
    // ORIG 0x8442f66-0x8442f77 `mov $0x51eb851f,%edx; mul; shr $0x5,%edx`
    // 商原地留 %edx ⇒ 字段与无符号商同号（int 字段恒产 mov %edx,%eax 中转，
    // 探针 /tmp/w8lt_mul3.o；unsigned 探针 /tmp/w8lt_mul2.o）。
    sig->m_regDay = day / 100;
    sig->m_regDate = reg;
    return SendResult(userIdx, slot, sig);
