// ORIG 0x0843ccf2 sz 0x4a7：GetDBHandle(8,0) → Acquire("DBThread.cpp",0x9129)
// → CStreamGuard(stream,true) → <<0x217 → <<slot →
// GetInBuffer<SIG_REQUEST_JOIN_GUILD_DB_REPLY> → GetOutBuffer<
// SIG_REQUEST_JOIN_GUILD> → 查 guild_join_list(charac_no=+0x18, server_group=
// +0x20)：exec 失败返 0；有行 → reply flag=0x12 → put(DISPATCH_Q) 返 0；
// 无行 → 查 guild_info(server_id=+0x20, guild_name=+0x00)：exec 失败返 0；
// 无行 → reply flag=0x15 → put 返 1；有行 fetch 失败返 0 → guild_id(0)/
// guild_name(1,0x17)/master_name(2,0x1e)/lev(3)/member_count(4) 任一失败返 0
// → 查 guild_join_list(guild_id)：exec 失败返 0；行数>=0x32 → reply flag=0x5f
// → put 返 1；<0x32 → escape_string(memo 源 +0x27) → inSert(guild_id, m_id=
// NumberToString(+0x1c), server_group=+0x20, charac_no=+0x18, born_year=+0x24,
// memo) → exec 失败返 0；affected==0 → reply flag=2 → put 返 1；否则 reply
// flag=0 → put 返 1。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0843ccf2
bool DB_RequestJoinGuild::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db;
    // [Lane31W8LTDBTaskView] 语句序照 ORIG（0x0843ccf2）：GetDBHandle 先于
    // StreamPool::Acquire/CStreamGuard 构造（ORIG `call GetDBHandle; mov 0x940bd6c,%eax;
    // … call Acquire; call CStreamGuard::C1` 顺序实证）。
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8, E_SERVER_GROUP_NONE);
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9129), true);
    SIG_REQUEST_JOIN_GUILD_DB_REPLY* reply;
    SIG_REQUEST_JOIN_GUILD* sig;
    char escBuf[53];

    guard.operator*()->operator<<((int)0x217);
    guard.operator*()->operator<<(slot);
    reply = /*SIG_REQUEST_JOIN_GUILD_DB_REPLY*/(
        guard.operator->()->GetInBuffer<SIG_REQUEST_JOIN_GUILD_DB_REPLY>());
    sig = /*SIG_REQUEST_JOIN_GUILD*/(
        stream->GetOutBuffer<SIG_REQUEST_JOIN_GUILD>());
    // [Q5W8LT 2026-09-11 落地] ORIG 0x0843ccf2：死局部 `result`（全函数零引用）的
    // `movl $0x0,-0x1c(%ebp)` 发在 ORIG 第 45 条（GetOutBuffer 之后），而声明区版本
    // 落在第 22 条（guard 构造后）⇒ 把声明移到使用点（GetOutBuffer 之后）后
    // 本件 DIFF→**AE**（308=308 归一 + gap 0）。
    int result = 0;
    db->set_query("seLect * from guild_join_list where charac_no=%d and server_group=%d",
                  sig->m_characNo, sig->m_serverGroup);
    if (db->exec(true) != 1)
        return 0;
    // [P5S2W8LT r1] P196/P222 极性+块序：ORIG 0x0843ce05 `test %eax,%eax;
    //   setne %al; test %al,%al; je 0x0843ce3d`（正极性：有行 = fall-through，
    //   无行块在跳转目标 0x843ce3d），且 flag=0x12/return 0 块属**有行**分支
    //   （ORIG 0x843ce66 区 `movl $0x12,(%eax); …; mov $0x0,%ebx; jmp epilogue`）
    //   ⇒ 原 `if (get_n_rows() == 0) {无行}` 包裹形产 sete + 反向块序。
    if (db->get_n_rows() != 0)
    {
        reply->m_flag = 0x12;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 0;
    }
    {
        db->set_query("seLect guild_id,guild_name,master_name,lev,member_count from guild_info where server_id=%d and expire_flag=0 and guild_name='%s'",
                      sig->m_serverGroup, sig->m_guildName);
        if (db->exec(true) != 1)
            return 0;
        if (db->get_n_rows() == 0)
        {
            reply->m_flag = 0x15;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            return 1;
        }
        if (db->fetch() != 1)
            return 0;
        if (db->get_uint(0, reply->m_guildId) != 1)
            return 0;
        if (db->get_str(1, reply->m_guildName, 0x17) != 1)
            return 0;
        if (db->get_str(2, reply->m_masterName, 0x1e) != 1)
            return 0;
        if (db->get_int(3, reply->m_lev) != 1)
            return 0;
        if (db->get_int(4, reply->m_memberCount) != 1)
            return 0;
        db->set_query("seLect * from guild_join_list where guild_id=%d",
                      reply->m_guildId);
        if (db->exec(true) != 1)
            return 0;
        // [A04-r5h · 轴 B/P196] ORIG 0x0843d026-0x0843d02e 为
        // `call get_n_rows; cmp $0x31,%eax; **seta** %al; test %al,%al; je 0x843d05f`
        // ⇒ 真形 = **正极性无符号** `get_n_rows() > 0x31`（then=0x5f 块落 fall-through、
        // body 在跳转目标 0x843d05f）。原 `get_n_rows() < 0x32`（负极性 + signed）使 4.4
        // 产 `setle` 且块序反 ⇒ 按 ORIG 改正极性比较（ORIG 为无符号序 ⇒ P196 前置通过）。
        // [J4S1 P2/P196] ORIG 0x0843d026 `cmp $0x31,%eax; seta %al`（无符号序）
        // ⇒ 需显式无符号视图，否则 4.4 产 setg。
        if ((unsigned int)db->get_n_rows() > 0x31u)
        {
            reply->m_flag = 0x5f;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            return 1;
        }
        db->escape_string(escBuf, sig->m_memo);
        db->set_query("inSert into guild_join_list(guild_id,m_id,server_group,charac_no,born_year,memo,occ_time) values(%d,%s,%d,%d,'%s','%s',now())",
                      reply->m_guildId, NumberToString(sig->m_mId, 0),
                      sig->m_serverGroup, sig->m_characNo,
                      sig->m_bornYear, escBuf);
        if (db->exec(true) != 1)
            return 0;
        if (db->getAffectedRowCount() == 0)
        {
            reply->m_flag = 2;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            return 1;
        }
        reply->m_flag = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
}