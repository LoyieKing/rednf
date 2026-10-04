// ============================================================================
// DB_InsertResponseTime::dispatch —— ORIG 0x0843fc26（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(4,0) → GetOutBuffer<SIG_INSERT_RESPONSE_TIME> → 循环 i<=99 且
// i<count：values 非空先 += ","；sprintf("(%d,from_unixtime(%d),%u,%u,%llu,
// %u)", channel_no(G_CEnvironment()->get_channel_no()), baseSec, id, count,
// bytesLow|bytesHigh, avg)；values += rowBuf。尾部 monthBuf[10]={0}；
// occTime=baseSec；localtime_r；strftime("%Y%m")；
// set_query("inSert into log_response_time_%s(channel_no,occ_time,packet_id,
// packet_count,total_response_time,avg_response_time) values%s", monthBuf,
// values.c_str()) → exec(true)（结果跨 values 析构保留）→ 返回 exec 结果。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0843fc26
// [r3] db/sig 改「声明即初始化」并前置于 STL 局部之前（ORIG 0843fc4f db→
//   0843fc5d sig→values ctor 序）⇒ ndiff 62→42、gap 2 不变；残余 = ORIG
//   4.1 lea/add 地址重物化 + 槽号（N18a/N82）⇒ 封顶。
bool DB_InsertResponseTime::dispatch(int /*userIdx*/, int /*slot*/, Stream* stream)
{
    // [J4S2-① P212/P309/P198①] ORIG 取得/物化序 = db(-0x24) → sig(-0x20) → i(-0x1c)：
    // 0x0843fc4f `mov %eax,-0x24`（db）→ 0x0843fc5d `mov %eax,-0x20`（sig）→
    // 0x0843fcc6 `movl $0x0,-0x1c`（i，位于 values 构造与 rowBuf 清零之后）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    SIG_INSERT_RESPONSE_TIME* sig =
        stream->GetOutBuffer<SIG_INSERT_RESPONSE_TIME>();
    int i;                          // -0x1c（ORIG 0843fcc6 movl $0x0,-0x1c）
    std::string values;             // -0x28
    // [P5S2W8LT r1+r3] ORIG 的 monthBuf(-0x32) 清零**不在声明处**：0x0843fc6b 起
    // 只对 rowBuf(-0x163) 发 rep stos，monthBuf 的
    // `movl $0,-0x32; movl $0,-0x2e; movw $0,-0x2a` 三条发在 0x0843fdfd-0x0843fe0b
    // （**循环结束后、occTime=baseSec 之前**）。声明处初始化器会把它们提前到
    // values ctor 之后 ⇒ 与 ORIG 双错位（r1 删初始化器后 fdiff 显示 ORIG-only 3 条）；
    // 故裸声明 + 循环后显式 memset 复现发码位。
    char monthBuf[10];              // -0x32（ORIG 无初始化器）
    time_t occTime;                 // -0x38
    struct tm tmBuf;                // -0x64
    char rowBuf[0xff] = { '\0' };   // -0x163（ORIG 255 字节带对齐分支 rep stos）
    i = 0;
    while (i <= 99 && i < sig->m_count)
    {
        if (!values.empty())
            values += ",";
        // ORIG 行字段逐次重读（P48 无 CSE）；%llu 直接读 8 字节（低字先行）。
        sprintf(rowBuf, "(%d,from_unixtime(%d),%u,%u,%llu,%u)",
                G_CEnvironment()->get_channel_no(),
                sig->m_baseSec,
                sig->m_entries[i].m_id,
                sig->m_entries[i].m_count,
                *(unsigned long long*)&
                    sig->m_entries[i].m_bytesLow,
                sig->m_entries[i].m_avg);
        values += rowBuf;
        ++i;
    }
    memset(monthBuf, 0, 0xa);   // ORIG 0x0843fdfd-0x0843fe0b
    occTime = (time_t)sig->m_baseSec;
    localtime_r(&occTime, &tmBuf);
    strftime(monthBuf, 10, "%Y%m", &tmBuf);
    db->set_query(
        "inSert into log_response_time_%s(channel_no,occ_time,packet_id,packet_count,total_response_time,avg_response_time) values%s",
        monthBuf, values.c_str());
    return db->exec(true);
}