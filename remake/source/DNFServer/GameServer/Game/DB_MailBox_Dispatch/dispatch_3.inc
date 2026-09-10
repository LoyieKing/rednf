bool DB_ReqQueryCharacInfoMailbox::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, (ENUM_SERVER_GROUP)0);
    SIG_QUERY_CHARAC_INFO* sig = stream->GetOutBuffer<SIG_QUERY_CHARAC_INFO>();

    // P212 候选实验（NL31）：块内局部全部提头声明（未初始化），初始化语句
    // 留在 ORIG 原位——4.4 全头部声明时槽分配与 4.1 同为"先声明深"单调。
    // 声明序=ORIG 槽深序（get_int 列序≠声明序；ORIG movl 序 -0x2c,-0x30,
    // -0x34,-0x38,-0x1c(dummy),-0x3c,-0x40 ⇒ job,lev,growType,characNo,
    // pad_impl,sex,mId）。
    int deleteFlag;
    int job;
    int lev;
    int growType;
    int characNo;
    int pad_impl;
    int sex;
    unsigned int mId;

    char name[0x3c] = { '\0' };
    db->escape_string(name, sig->m_name);
    db->set_query(
        "seLect delete_flag,m_id,charac_no,job,lev,grow_type,sex from charac_info where charac_name='%s'",
        name);
    // ORIG 0x0843be5b/0x0843bebe：失败路径 `call _sendResult` 后直接
    // `jmp +0x3e6`（跳过 mov $1,%eax）⇒ 原码是 `return _sendResult(...)`
    // （_sendResult 返 bool，无 int→bool 归一化）。
    if (!db->exec(true))
        return _sendResult(2, slot, 0, 0, 0, 0, 0, 0);

    if (db->get_n_rows() == 0)
        return _sendResult(0x15, slot, 0, 0, 0, 0, 0, 0);
    // P48 双调用：ORIG（0x0843bec3 `setne %al; je 0843c0dd`）第二次判定是
    // `!= 0` 正向跳成功链、`== 0` 落入尾部 0x15 块（fallthrough）→ 按该形态
    // 转写（行为与两连 `==0` 块等价；块形状影响 verdict 序列）。
    if (db->get_n_rows() != 0)
    {
        deleteFlag = 0;
        if (!db->fetch() || !db->get_int(0, deleteFlag))
            return _sendResult(2, slot, 0, 0, 0, 0, 0, 0);
        if (deleteFlag != 0)
            return _sendResult(0x15, slot, 0, 0, 0, 0, 0, 0);

        // 宽度对齐 ORIG（报告 0x0843c02a..0x0843c092）：列 2..6 callee =
        // MySQL::get_int(int, int&)@0x0811692c → 局部须为 int（原 unsigned int
        // 会绑 0x080e22ae 转发重载，callee 不符）；列 1 m_id callee @0x080e22ae
        // = get_int(int, unsigned int&) → mId 保持 unsigned int。
        // ORIG（+0x2a0 `movl $0x0,-0x1c(%ebp)`）在此列初始化块多零写入一个 dummy
        // 槽（只写不读，N82 遗留）。以同块内一个只写不读的局部复刻该条死 movl。
        job = 0;
        lev = 0;
        growType = 0;
        characNo = 0;
        pad_impl = 0;
        sex = 0;
        mId = 0;
        db->get_int(1, mId);
        db->get_int(2, characNo);
        db->get_int(3, job);
        db->get_int(4, lev);
        db->get_int(5, growType);
        db->get_int(6, sex);

        // ORIG（0843c0bd mov -0x20(%ebp),%eax）：第 3 参 name 传的是 GetOutBuffer
        // 返回的 sig 指针，NOT 转义后的 name 缓冲（0843c0bb..0843c0bd 用 -0x20 槽）。
        _sendResult(0, slot, (const char*)sig, characNo, job, lev, growType, sex);
        goto done;
    }

    _sendResult(0x15, slot, 0, 0, 0, 0, 0, 0);
done:
    return 1;
