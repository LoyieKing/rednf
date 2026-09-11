// ============================================================================
// DB_ReloadAutoPunishRuleBackIP::dispatch —— ORIG 0x0842cdf6
// GetDBHandle(DB_HANDLE_6,0) → seLect ip,start_ip,end_ip from
// auto_punish_blackip_info where apply_flag=1 limit %d(0x7d0)；exec 失败返
// 0。回包流 0x108/-1 + SIG_AUTO_PUNISH_RULE_BLACK_IP{+0 ushort count}。
// 逐行 fetch：get_str(0,ipStr,0xc)、get_uint(1,startIp)、get_uint(2,endIp)
// 任一失败返 0；OS_API::snprintf(ipBuf,0x10,"%s.0",ipStr) →
// inet_addr 写 +0x04+i*8、start 写 +0x08+i*8、end 写 +0x09+i*8。
// put(DISPATCH_Q) 返 1。
// （N 封顶：ndiff 89 残差为帧 -4 偏移 + seta/setl 符号比较极性 + P183
// 地址重物化，见报告 dispatch.md）
// ============================================================================
bool DB_ReloadAutoPunishRuleBackIP::dispatch(int userIdx, int slot,
                                             Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect ip,start_ip,end_ip from auto_punish_blackip_info where apply_flag=1 limit %d",
        0x7d0);
    if (!db->exec(true))
        return 0;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7059), true);
    guard.operator*()->operator<<(0x108);
    guard.operator*()->operator<<(-1);
    // [b23S3C] ORIG 0x0842cee3 形：get_n_rows 结果直 `mov %eax,%edx; mov %dx,(%eax)`
    // 落 out+0（无 rowCount 具名槽，P0/P48），且循环条件 ORIG 0x0842d043 **每轮重取**
    // get_n_rows 并按无符号比较（`cmp -0xc(%ebp),%eax; seta`）⇒ i 改 unsigned int、
    // 界改内联 call；ipStr ORIG 清零 15 字节（`movl×3 + movw + movb`）⇒ 数组改 0xf。
    DB_AutoPunishBlackIpView* out = reinterpret_cast<DB_AutoPunishBlackIpView*>(
        guard->GetInBuffer<SIG_AUTO_PUNISH_RULE_BLACK_IP>());
    out->m_count = (unsigned short)db->get_n_rows();
    for (unsigned int i = 0; i < (unsigned int)db->get_n_rows(); ++i)
    {
        if (!db->fetch())
            return 0;
        char ipStr[0xf] = {};
        unsigned int startIp = 0;   // [b23S3C] ORIG 0x0842cf37/-0x30,-0x34 两清零在
        unsigned int endIp = 0;     // get_str 之前（4.1 初始化前置）⇒ 声明上移

        if (!db->get_str(0, ipStr, 0xc))
            return 0;
        if (!db->get_uint(1, startIp))
            return 0;
        if (!db->get_uint(2, endIp))
            return 0;
        char ipBuf[0x10] = {};
        OS_API::snprintf(ipBuf, 0x10, "%s.0", ipStr);
        out->m_entries[i].m_ip = inet_addr(ipBuf);
        out->m_entries[i].m_start = (unsigned char)startIp;
        out->m_entries[i].m_end = (unsigned char)endIp;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}