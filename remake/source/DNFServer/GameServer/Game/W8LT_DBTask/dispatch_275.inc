// ============================================================================
// DB_ReloadAutoPunishRuleHackType::dispatch —— ORIG 0x0842d16c
// GetDBHandle(DB_HANDLE_6,0) → seLect hack_type,cnt,etc,hack_sub_type,
// hack_sub_cnt,apply_flag, ip_cnt from auto_punish_hack_info where
// apply_flag > 0；exec 失败返 0。回包流 0x109/-1 + SIG_AUTO_PUNISH_RULE_
// HACK_TYPE，memset 0x2720。行循环 i < min(0x1f4, get_n_rows())：fetch +
// get_uint(0..1)、get_ulonglong(2,etc)、get_uint(3..6) 任一失败返 0。
// hackType==0 → 头部 +0x00=cnt、+0x04/+0x08=etc 低/高，跳过条目；
// 0x64<hackType<=0xfa0 且 (hackSubType==0 或 0x64<hackSubType<=0xfa0)
// → hackSubType==0 时 hackSubCnt=0，写条目（ushort hackType/cnt/
// hackSubType/hackSubCnt/applyFlag + uint ipCnt/etcLo），m_count++；
// 否则跳过该行。put(DISPATCH_Q) 返 1。
// （N 封顶：ndiff 394 残差为 lea/movzwl/shl/add 地址重物化(P183) + 帧偏移，
// 见报告 dispatch.md）
// ============================================================================
// [b28d-4] DIFF：S 档 gap=40（缺失转写/调用）+ N82 槽序；ORIG 0842d16c
bool DB_ReloadAutoPunishRuleHackType::dispatch(int userIdx, int slot,
                                               Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect hack_type,cnt,etc,hack_sub_type,hack_sub_cnt,apply_flag, ip_cnt from auto_punish_hack_info where apply_flag > 0");
    if (!db->exec(true))
        return 0;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7094), true);
    guard.operator*()->operator<<(0x109);
    guard.operator*()->operator<<(-1);
    // [Lane31W8LTDBTaskView] 权威头直取（去 cast）。
    SIG_AUTO_PUNISH_RULE_HACK_TYPE* out =
        guard.operator->()->GetInBuffer<SIG_AUTO_PUNISH_RULE_HACK_TYPE>();
    memset(out, 0, 0x2720);
    // [b28d-TakeW8LT 形复原] ORIG 0x0842d586 每轮循环**重算**循环上界：
    // `movl $0x1f4,-0x1c; call get_n_rows; mov %eax,-0x18; lea -0x1c,0x4(%esp);
    //  lea -0x18,(%esp); call std::min<unsigned int>`（0x0842d5a8 的
    // _ZSt3minIjERKT_S2_S2_）⇒ 源码形 = min 内联在 for 条件里、get_n_rows 在
    // 第一实参位（旧形提升到循环外，少发每轮 9 条）。
    for (unsigned int i = 0; i < std::min<unsigned int>(db->get_n_rows(), 0x1f4); ++i)
    {
        if (!db->fetch())
            return 0;
        unsigned int hackType = 0;
        unsigned int cnt = 0;
        unsigned long long etc = 0;
        unsigned int hackSubType = 0;
        unsigned int hackSubCnt = 0;
        unsigned int applyFlag = 0;
        unsigned int ipCnt = 0;
        if (!db->get_uint(0, hackType))
            return 0;
        if (!db->get_uint(1, cnt))
            return 0;
        if (!db->get_ulonglong(2, etc))
            return 0;
        if (!db->get_uint(3, hackSubType))
            return 0;
        if (!db->get_uint(4, hackSubCnt))
            return 0;
        if (!db->get_uint(5, applyFlag))
            return 0;
        if (!db->get_uint(6, ipCnt))
            return 0;
        if (hackType == 0)
        {
            out->m_field0 = cnt;
            // [K5W8LTDBTask W8DiagD 负结论] 旋钮 `(unsigned int)(etc & 0xffffffffULL)`
            // → `(unsigned int)etc` 实测无变化（our 336 未动，四数 141/1750/12/57
            // 逐值相同）⇒ 4.4 两形同码；ORIG 0x0842d402-0x0842d40e 的 9 条短形
            // 属 4.1 后端，P246/P92 族不可迁。回滚保原式。
            out->m_field4 = (unsigned int)(etc & 0xffffffffULL);
            out->m_field8 = (unsigned int)(etc >> 32);
            continue;
        }
        if (hackType <= 0x64 || hackType > 0xfa0)
            continue;
        if (hackSubType != 0 &&
            (hackSubType <= 0x64 || hackSubType > 0xfa0))
            continue;
        if (hackSubType == 0)
            hackSubCnt = 0;
        // [b28d-TakeW8LT 形复原] ORIG 0x0842d45f..d563 七个写入站点**各自**
        // 重读 out+0xc 的 m_count 重算索引（ORIG movzwl 15/shl 14 vs 旧形 3/2），
        // 且写入顺序 = +0x10,+0x12,+0x14,+0x16,+0x1c,+0x18,+0x20（ipCnt 在
        // applyFlag 之前，Ghidra 反编译同序）。
        out->m_entries[(unsigned int)out->m_count].m_hackType =
            (unsigned short)hackType;
        out->m_entries[(unsigned int)out->m_count].m_cnt = (unsigned short)cnt;
        out->m_entries[(unsigned int)out->m_count].m_hackSubType =
            (unsigned short)hackSubType;
        out->m_entries[(unsigned int)out->m_count].m_hackSubCnt =
            (unsigned short)hackSubCnt;
        out->m_entries[(unsigned int)out->m_count].m_ipCnt = ipCnt;
        out->m_entries[(unsigned int)out->m_count].m_applyFlag =
            (unsigned short)applyFlag;
        out->m_entries[(unsigned int)out->m_count].m_etcLo =
            (unsigned int)(etc & 0xffffffffULL);
        out->m_count = (unsigned short)(out->m_count + 1);
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}