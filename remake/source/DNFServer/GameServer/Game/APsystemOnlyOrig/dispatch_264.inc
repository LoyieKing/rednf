// ============================================================================
// DB_LoadBreakAwayPreventData::dispatch —— ORIG 0x084389fe
// GetDBHandle(DB_HANDLE_6,0) → >>flag(bool)。SIG_BREAK_AWAY_PREVENT_SYSTEM_
// MANAGER 栈对象（ctor 清零 0x2e4）。seLect churn_system_manager 最新行；
// exec 失败或行数!=1 返 0；fetch + 逐列读：no(int)、weekday_var_a..z(short
// ×6)、next_reward_day(short)、state_flag(byte)。flag==0 → UPDATE
// state_flag=0 where no（exec 失败/无影响行返 0；成功也返 0——ORIG 两路皆
// 返 0 不再加载）。flag!=0 → UPDATE state_flag=1（失败返 0）→ 置
// sig.m_fieldE=flag → seLect churn_reward_manager 全表，逐行 fetch 后按
// 步长 0x18 填 m_rewards[i]（quest_id/min_day/max_day 为 byte、
// min_val/max_val/luck_point 为 int、item_id 为 uint、add_info 为 int）。
// 回包流 0x141/-1 + GetInBuffer<SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER> +
// memcpy 0x2e4 → put(DISPATCH_Q) 返 1。
// ============================================================================
// [b28d-4] DIFF：P219/P248：字节字段物化/同型化差（ORIG movzbl vs 4.4 mov/shr/and）；ORIG 084389fe
// [r3 负结论] `if (rowCount == 1) {} else { return 0; }` 空-if 极性包裹（P222）
//   实测**无效**（484/484、ndiff 8、差集 {jne}/{je} 全同）：ORIG 08438c76 `jne`
//   跳后部 return-0 块 + 两处 tail-merge 块 vs 4.4 本地 `je` ⇒ N 封顶。
// [R5BigTURest 2026-09-11 复核] 已落 1 处：两处 int 零初始化次序（ORIG
//   `movl $0x0,-0x14` 先 / `-0x40` 后 ⇒ 源码 rowCount 声明须在 no 之前）⇒
//   ndiff 4→2（484/485、gap -1 不变）。残留 2 处均为 N：① ORIG 08438c76
//   `jne` + 后部 return-0 块 vs 4.4 `je` + 目标后置块（块布局）；② ORIG
//   0843?? 两处**重复** `mov $0x0,%ebx; jmp` 出口块（4.1 不合并），4.4 合并
//   为一份 ⇒ 源码不可造。
bool DB_LoadBreakAwayPreventData::dispatch(int userIdx, int slot,
                                           Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    bool flag = false;
    *stream >> flag;
    SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER sig;
    // [R5BigTURest 偏移/槽序] ORIG 0x08438c5x 两处 int 零初始化次序为
    // `movl $0x0,-0x14(%ebp)`（rowCount，后接 `cmpl $0x1,-0x14(%ebp)` 判行数）
    // **先**、`movl $0x0,-0x40(%ebp)`（no，后作 UPDATE 第二实参）**后**
    // ⇒ 源码中 rowCount 声明在 no 之前（4.4 按声明序发零初始化）。
    int rowCount = 0;
    int no = 0;
    db->set_query(
        "seLect no, weekday_var_a, weekday_var_b, weekday_var_c, weekend_var_x, weekend_var_y, weekend_var_z, next_reward_day, state_flag from churn_system_manager order by no desc limit 1");
    if (!db->exec(true))
        return 0;
    rowCount = db->get_n_rows();
    if (rowCount != 1)
        return 0;
    if (!db->fetch())
        return 0;
    if (!db->get_int(0, no))
        return 0;
    if (!db->get_short(1, reinterpret_cast<short&>(sig.m_field0)))
        return 0;
    if (!db->get_short(2, reinterpret_cast<short&>(sig.m_field2)))
        return 0;
    if (!db->get_short(3, reinterpret_cast<short&>(sig.m_field4)))
        return 0;
    if (!db->get_short(4, reinterpret_cast<short&>(sig.m_field6)))
        return 0;
    if (!db->get_short(5, reinterpret_cast<short&>(sig.m_field8)))
        return 0;
    if (!db->get_short(6, reinterpret_cast<short&>(sig.m_fieldA)))
        return 0;
    if (!db->get_short(7, reinterpret_cast<short&>(sig.m_fieldC)))
        return 0;
    if (!db->get_byte(8, reinterpret_cast<char&>(sig.m_fieldE)))
        return 0;
    if (!flag)
    {
        db->set_query(
            "upDate churn_system_manager set state_flag=%d where no = %d",
            // [b28d-BigW8LT 转写复原] ORIG 0x08438c8f 两路 UPDATE 的第一实参
            // 都是 `movzbl -0x39(%ebp)`（**flag 变量本身**，非字面量 0/1）：
            // `mov -0x40(%ebp),%edx; movzbl -0x39(%ebp),%eax; movzbl %al,%eax;
            //  mov %edx,0xc(%esp); mov %eax,0x8(%esp)`。字面量形少发 2 组
            // movzbl+mov（实测助记符 census movzbl 6/2）。bool 取值域 {0,1}
            // ⇒ 两路传 flag 与原传常量**同值**，行为不变。
            flag, no);
        if (!(db->exec(true) && db->getAffectedRowCount() != 0))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x8684, 0)(
                "BREAK_AWAY : STATE_FALG ALREADY OFF!!");
        }
        // ORIG 0x08438d25 为**单一** return 0 站点：0x08438ceb `test %al,%al;
        // je 08438d25` 跳过 trace 直落此处，成功/失败同路返回（Ghidra 反编译
        // 亦只有一个 `return 0;` 在 if(bVar1) 之后）。
        return 0;
    }
    db->set_query(
        "upDate churn_system_manager set state_flag=%d where no = %d", flag, no);
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x868a, 0)(
            "BREAK_AWAY : STATE_FALG ALREADY ON!!");
        // ORIG：ON 分支 UPDATE 失败/无影响行时打日志后继续执行（不返回）
    }
    sig.m_fieldE = (unsigned char)flag;
    db->set_query(
        "seLect min_day, max_day, min_val, max_val, item_id, add_info, luck_point, quest_id from churn_reward_manager");
    if (!db->exec(true))
        return 0;
    rowCount = db->get_n_rows();
    for (int i = 0; i < rowCount; ++i)
    {
        if (!db->fetch())
            return 0;
        // [b28d-BigW8LT 寻址复原] 旧形提升出 `row = &sig.m_rewards[i]` 行指针
        // 局部，ORIG 0x084389fe 无此局部——8 个站点各自重算
        // `i*0x18 + &sig + 字段偏移`（实测 ORIG add 38/shl 8/lea 39 vs 我方
        // 18/1/25，缺的 49 条全在此）。改回逐站点下标式成员访问。
        if (!db->get_byte(
                0, reinterpret_cast<char&>(sig.m_rewards[i].m_field1)))  // min_day
            return 0;
        if (!db->get_byte(
                1, reinterpret_cast<char&>(sig.m_rewards[i].m_field2)))  // max_day
            return 0;
        if (!db->get_int(2, sig.m_rewards[i].m_field4))    // min_val
            return 0;
        if (!db->get_int(3, sig.m_rewards[i].m_field8))    // max_val
            return 0;
        if (!db->get_uint(4, (unsigned int&)sig.m_rewards[i].m_fieldc))  // item_id
            return 0;
        if (!db->get_int(5, sig.m_rewards[i].m_field10))   // add_info
            return 0;
        if (!db->get_int(6, sig.m_rewards[i].m_field14))   // luck_point
            return 0;
        if (!db->get_byte(
                7,
                reinterpret_cast<char&>(sig.m_rewards[i].m_field0)))  // quest_id
            return 0;
    }
    // [K5W8LTDBTask W8DiagC 语义修正 ③⑨] ORIG 0x08439063-0x08439066
    //   `mov -0x14(%ebp),%eax; mov %eax,-0x4c(%ebp)`
    // —— sig 栈对象基址 -0x32c（ctor `lea -0x32c(%ebp)` @0x08438a3f，回包
    //    `memcpy(out,&sig,0x2e4)` 同址）⇒ -0x4c = sig+0x2e0 = m_field2e0；
    //    -0x14 = rowCount（循环上界，来自第二次 get_n_rows）。
    // 该写无我方替代路径（-0x4c 全函数只此一写）；缺失使我方回包 m_field2e0
    // 恒为 ctor 清零值 0 ⇒ 消费方 GlobalData.cpp:5870
    //   `for (i=0; i<((const SIG_...*)this)->m_field2e0; ++i)` 循环 0 次，
    //   断线重连奖励表整表不装载（真语义 bug）。
    sig.m_field2e0 = rowCount;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x86c5), true);
    guard.operator*()->operator<<(0x141);
    guard.operator*()->operator<<(-1);
    SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER* out =
        guard->GetInBuffer<SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER>();
    memcpy(out, &sig, 0x2e4);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}