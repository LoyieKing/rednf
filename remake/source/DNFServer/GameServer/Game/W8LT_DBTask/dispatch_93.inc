bool DB_LoadDimension::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    // 声明序照 ORIG 栈槽（低→高）：guard(-0x2c) db(-0x24) sig(-0x20)
    // out(-0x1c)；guard 于末尾 put 前构造（ORIG 同点）。
    // [L4-r2 宽度旋钮已实测回退] widthscan clr_b/cmp_b = orig 0 / our 2 曾疑为
    // 「我方偏窄」，改 `int bVar1` 后 gap/四数不变而 ndiff 107→**109 反劣** ⇒
    // 该 byte 形非局部宽度问题（归 P292/N82 槽分区，见 Main 判据⑫ 反误判条）。
    MySQL* db =
        GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    SIG_LOAD_DIMENSION* sig = stream->GetOutBuffer<SIG_LOAD_DIMENSION>();
    db->set_query(
        "seLect dungeon1, dungeon2, dungeon3, dungeon4, dungeon5, dungeon6 from charac_dimension_inout where charac_no=%u",
        sig->m_field00);
    // [A04-r5] ORIG 0x08431f60-0x08431f8c：`call exec; xor $0x1,%eax; test %al,%al;
    // jne L_true` + `call fetch; xor $0x1; test; je L_false` + 常量岛
    // （mov $1/jmp/mov $0）汇聚 + `test %al,%al; je <get_byte 段>` ⇒ **零槽短路
    // 物化**（值只走 %eax，无 -0x25 槽）⇒ 原码无具名 bool 局部（P0），按 P226/P316
    // 用语句表达式包住整个条件（具名 bool 形 4.4 会落 `movb $0,-0x25` + `cmpb`）。
    if (({ db->exec(true) != 1 || db->fetch() != 1; }))
    {
        db->set_query(
            "inSert into charac_dimension_inout(charac_no,dungeon1,dungeon2,dungeon3,dungeon4,dungeon5,dungeon6) values(%u,%d,%d,%d,%d,%d,%d)",
            sig->m_field00,
            (int)(char)sig->m_dimensionInout[0],
            (int)(char)sig->m_dimensionInout[1],
            (int)(char)sig->m_dimensionInout[2],
            (int)(char)sig->m_dimensionInout[3],
            (int)(char)sig->m_dimensionInout[4],
            (int)(char)sig->m_dimensionInout[5]);
        if (db->exec(true) != 1)
            return 0;
    }
    else
    {
        // [A04-r5] ORIG 0x0843204e 起 6 个 get_byte 各发独立
        // `call; xor $0x1; test %al,%al; je <下一个>; mov $0x0,%ebx; jmp <epilogue>`
        // （每个失败出口独立早退）⇒ 必须拆成 6 条独立 `if (...!=1) return 0;`
        // （P133：合并 `||` 链在 4.4 折成单条 jne 汇聚，少 5 组早退岛）。
        if (db->get_byte(0, sig->m_dimensionInout[0]) != 1)
            return 0;
        if (db->get_byte(1, sig->m_dimensionInout[1]) != 1)
            return 0;
        if (db->get_byte(2, sig->m_dimensionInout[2]) != 1)
            return 0;
        if (db->get_byte(3, sig->m_dimensionInout[3]) != 1)
            return 0;
        if (db->get_byte(4, sig->m_dimensionInout[4]) != 1)
            return 0;
        if (db->get_byte(5, sig->m_dimensionInout[5]) != 1)
            return 0;
    }
    db->set_query(
        "seLect dungeon1 from charac_blood_inout where charac_no=%u",
        sig->m_field00);
    if (({ db->exec(true) != 1 || db->fetch() != 1; }))
    {
        db->set_query(
            "inSert into charac_blood_inout(charac_no,dungeon1) values(%u,%d)",
            sig->m_field00, (int)(char)sig->m_limitInoutCount);
        if (db->exec(true) != 1)
            return 0;
    }
    else
    {
        if (db->get_byte(0, sig->m_limitInoutCount) != 1)
            return 0;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7955), true);
    guard.operator*()->operator<<(0x14e);
    guard.operator*()->operator<<(slot);
    SIG_LOAD_DIMENSION* out = guard->GetInBuffer<SIG_LOAD_DIMENSION>();
    memcpy(out, sig, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
