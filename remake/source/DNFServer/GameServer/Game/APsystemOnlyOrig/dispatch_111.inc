// ============================================================================
// DB_BlackIPMonitor —— ORIG dispatch 0x08432ad2（报告 dispatch.md）。
// GetDBHandle(DB_HANDLE_6,0) → set_query(ip_monitor_punish 全表) →
// exec 失败返 0 → cMyTrace(0x7a03) 打行数 → 循环（条件每轮重取
// get_n_rows()）：fetch 失败返 0；满 100 行或末行时组包
// Acquire("DBThread.cpp",0x7a28) → <<0x156 → <<-1 →
// GetInBuffer<SIG_IP_MONITOR_PUNISH>：+0=count、+4..+0xc83=item 数组
// （行步长 0x20：ip[0x10]/type/m_id_cnt/start_time/end_time）、+0xc84=flag
// （首包 1 后续 0）→ put(DISPATCH_Q)。行缓冲 memset 0xc80、组包 memcpy 0xc80。
// （N 封顶：ndiff 263 残差为 lea/shl/add 地址重物化(P183) + cMyTrace 独立
// 槽(N82) + xor 极性，见报告 dispatch.md）
// ============================================================================
// [b28d-4] DIFF：P222：块序/极性（ORIG je 落点 vs 4.4 jne）；ORIG 08432ad2
bool DB_BlackIPMonitor::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect ip, type, m_id_cnt, unix_timestamp(start_time), unix_timestamp(end_time) from ip_monitor_punish ORDER BY start_time ASC");
    if (!db->exec(true))
        return 0;
    // P37 临时链式：ORIG 先 call get_n_rows 以 ebx 缓存（跨 cMyTrace 构造），
    // 再构造临时并以其为 operator() 实参。
    cMyTrace(__PRETTY_FUNCTION__, 0x7a03, 0)(
        "BLACKIP GET DB ROWS COUNT : %d", db->get_n_rows());
    unsigned int row = 0;        // -0x18 行索引（循环条件无符号比较 seta）
    // [b28d-TakeW8LT] ORIG 0x08432ad2 的 `cmpl $0x62,-0x14; ja` 是无符号比较
    // （旧形 int ⇒ 4.4 发 jg）⇒ 源码类型为 unsigned int。
    unsigned int buffered = 0;   // -0x14 当前包已缓冲行数
    // [SweepIntervalSet] P248 档②/P292：ORIG 0x0843xx `movzbl -0xd(%ebp),%eax;
    // xor $0x1,%eax; test %al,%al; je`（无 setcc）⇒ flag 为 bool（char 形被 4.4
    // 折成 `cmpb $0x0,-0xd; jne`，少 2 条）。
    bool flag = false;           // -0x0d 首包标志
    char buf[0xc80];             // -0xcb4 行缓冲（100 × 0x20）
    // [A04-r5] 判据⑫ clr_l orig=4/our=3：ORIG 0x08432b7b 与 0x08432b8d **两次**
    // `movl $0x0,-0x18(%ebp)`（row 声明初始化 + for init；P191 双初始化形），
    // 循环为 for 布局（init→jmp cond→body→increment@0x08432e53→cond@0x08432e57）。
    for (row = 0; db->get_n_rows() > row; ++row)
    {
        if (!db->fetch())
            return 0;
        if (buffered == 0)
            memset(buf, 0, 0xc80);
        // [b28d-TakeW8LT 形复原] ORIG 0x08432ad2 无 item 指针局部：5 个站点
        // 各自重算 `buf + buffered*0x20`（ORIG add 18/shl 5 vs 旧形 6/1）。
        if (!db->get_str(0, ((DB_IpMonitorPunishItem*)buf)[buffered].m_ip, 0x10))
            return 0;
        if (!db->get_int(1, ((DB_IpMonitorPunishItem*)buf)[buffered].m_type))
            return 0;
        if (!db->get_uint(2, ((DB_IpMonitorPunishItem*)buf)[buffered].m_idCnt))
            return 0;
        if (!db->get_uint(3, ((DB_IpMonitorPunishItem*)buf)[buffered].m_startTime))
            return 0;
        if (!db->get_uint(4, ((DB_IpMonitorPunishItem*)buf)[buffered].m_endTime))
            return 0;
        if (buffered <= 98 && db->get_n_rows() - 1 > row)
        {
            ++buffered;
        }
        else
        {
            CStreamGuard guard(
                GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7a28),
                true);
            guard.operator*()->operator<<(0x156);
            guard.operator*()->operator<<(-1);
            // [Lane31W8LTDBTaskView] 视图 cast 反模式→权威头具名直访问（ORIG
            // 0x08432ad2：`mov %eax,0x0(%edx)` 等 RHS-first；cast 形在 4.4 下
            // base 先入 callee-saved 跨 call）。
            SIG_IP_MONITOR_PUNISH* sig =
                guard.operator->()->GetInBuffer<SIG_IP_MONITOR_PUNISH>();
            sig->m_count = buffered + 1;
            memcpy(&sig->m_items[0], buf, 0xc80);
            if (!flag)
            {
                flag = true;
                // [J4S1 P73/P136] ORIG 0x08432deb `movb $1,-0xd` →
                // 0x08432df2 `movzbl -0xd,%edx` → `mov %dl,0xc84` ⇒ 原码经
                // bool 变量读回赋值（非直写常量）。
                sig->m_flag = flag;
            }
            else
            {
                sig->m_flag = 0;
            }
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            buffered = 0;
        }
    }
    return 1;
}