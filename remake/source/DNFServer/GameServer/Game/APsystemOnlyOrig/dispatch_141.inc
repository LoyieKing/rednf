// ============================================================================
// DB_LoadCleanPadPoint::dispatch —— ORIG 0x0842ecf4（报告
// docs/class_func_reports/DB_LoadCleanPadPoint/dispatch.md 全文）
// ============================================================================
// DB_HANDLE_6。seLect hack_type,value from hack_cleanpad_ratio_info；exec
// 失败 → logFormat(0x73b0) 返 0；get_n_rows()==0 → cMyTrace(0x73b6,0) 返 1；
// 否则 Acquire("DBThread.cpp",0x73be) → guard <<0x127(int) <<-1(int) →
// GetInBuffer<SIG_LOAD_CLEANPAD_POINT> memset 0x3cf0 → 逐行
// fetch/get_int(0,hack_type)/get_int(1,value)（任一失败
// s_stream_pool->Free(guard) 返 0）；100<hack_type<0xfa1 时
// m_value[hack_type-0x65]=value；循环毕 put(DISPATCH_Q,guard) 返 1。
bool DB_LoadCleanPadPoint::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    db->set_query("seLect hack_type,value from hack_cleanpad_ratio_info");
    if (db->exec(true) != 1)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x73b0,
                              "[QUERY_ERROR] select hack_cleanpad_ratio_info");
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        // ORIG 0x0842e…：trace 临时对象槽 -0x24（比 guard -0x2c 近 8B）；
        // 具名对象使 4.4 两槽互换（CountOfHumanCertify 同法，P37）。
        cMyTrace(__PRETTY_FUNCTION__, 0x73b6, 0)(
            "[QUERY_NOTICE] have no data hack_cleanpad_ratio_info");
        return 1;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x73be), true);
    guard.operator*()->operator<<((int)0x127);
    guard.operator*()->operator<<((int)-1);
    DB_CleanpadRatioView* out = reinterpret_cast<DB_CleanpadRatioView*>(
        guard.operator->()->GetInBuffer<SIG_LOAD_CLEANPAD_POINT>());
    // ORIG 0x0842e…：`mov %eax,-0x10; mov -0x10,%eax(design 先评估);
    // movl $0x3cf0; movl $0; mov -0x10,%eax; mov (%esp)` ⇒ 首参先评估
    // 后重载（4.1 左起评估序）。数组 decay 形试验。
    memset(out->m_value, 0, 0x3cf0);
    // ORIG 循环变量为 uint（cmp/seta 无符号比较；int 会生成 setg）。
    for (unsigned int i = 0; i < (unsigned int)db->get_n_rows(); ++i)
    {
        int hackType = 0;
        int value = 0;
        if (db->fetch() != 1 || db->get_int(0, hackType) != 1 ||
            db->get_int(1, value) != 1)
        {
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        // ORIG 0x0842ef0a-0x0842ef30 双 jmp 岛（probe pad_p2 实证）：
        // `cmp $0x64;jle island1` + `cmp $0xfa0;jg island2` + write 块尾
        // `jmp ef31`；island1=nop;jmp、island2=nop(fall)。4.4 对正向
        // `if(a>b&&c<d)` 合并 fall-through 消 jmp ⇒ 唯显式 goto 复现。
        if (hackType <= 100)
            goto NEXT_ROW;
        if (hackType > 4000)
            goto NEXT_ROW;
        out->m_value[hackType - 0x65] = value;
    NEXT_ROW:;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}