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
    // [P5S2W8LT r1+r3，已被 R19a09 取代] ORIG 的 monthBuf(-0x32) 三条
    // `movl $0,-0x32; movl $0,-0x2e; movw $0,-0x2a` 发在 0x0843fdfd-0x0843fe0b
    // （循环结束后、occTime=baseSec 之前）；当时以「裸声明 + 循环后 memset」匹配位置，
    // 但 4.4 对 `memset(...,10)` 恒退化 `call memset@plt`（N78 反向，我方多出调用）。
    // [R19a09 §16.2/§24 定稿] 改聚合零初始化（探针 /tmp/verify357/R19a09/probe.cpp f2：
    //   `char b[10]={0};` ⇒ 4.4 发 `movl/movl/movw` 三条 = ORIG 同形）⇒ .o 内
    //   `memset@plt` 站点 1→0、件级 225→219 条（ORIG 191）；代价 = 三条落声明处
    //   （ORIG 落循环后，P212/P331 声明位族，指令内容同形）。
    char monthBuf[10] = { 0 };      // -0x32（ORIG 三条内联清零；R19a09 落修）
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
    occTime = (time_t)sig->m_baseSec;
    localtime_r(&occTime, &tmBuf);
    strftime(monthBuf, 10, "%Y%m", &tmBuf);
    db->set_query(
        "inSert into log_response_time_%s(channel_no,occ_time,packet_id,packet_count,total_response_time,avg_response_time) values%s",
        monthBuf, values.c_str());
    return db->exec(true);
}