// ============================================================================
// GetBingoEvent（ORIG T 0x08448bee）
// ============================================================================

// [b28d-solo-DB_Login] verdict=DIFF ndiff=2 gap=-1（N107：成员常量偏移取址 ORIG
// `lea 0x413c(%eax),%edx` vs 4.4 `add+mov`；P0 resetBase unsigned 化修装载时刻
// （5→2）。p167 CLEAR）。
// S216：union{unsigned int;int} 同址 int 视图实测仍产 add+mov（探针
// /tmp/s216_probe2.cpp fA/fB）⇒ 维持 N107（成员常量取址 lea-vs-add）封顶。
// [Lane26 复核] 再探具名数组成员取址 `arr[i].m_id`、union 数组视角 `u.m_ids[0]`
// （/tmp/l26p/pc.cpp t2/t3）实测仍 `add $0x413c,%eax; mov %eax,%edx`，4.4 对成员
// 取址恒 add、无 lea 路径 ⇒ 维持 N107；jcc/jmp 目标下标本件 bmm=8 全为位移差。
bool DB_Login::GetBingoEvent(SIG_LOGIN_DATA* sig)
{
    // [b28d-solo-DB_Login] P0/P140：ORIG 无 bingo 引用局部（地址直传 clear）、
    // 无 evt 局部/判空（GetRepeatEvent 返回值直传虚调用）。
    reinterpret_cast<SIG_BINGO_EVENT*>(SGBC(sig, OFF_BINGO_EVT))->clear();
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x6a)->IsEventing(0))
    {
        return 1;                            // 事件未开启：视为无数据成功
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    // [b28d-solo-DB_Login] P0：ORIG 无 curSec 具名槽（getCurSec 结果 eax 直传），
    // +0x37c 为 disp 直读（P82 视图；裸 cast+off 多一条 add）。
    struct EnvView { char m_pad[0x37c]; int m_dailyResetHour; };
    // [b28d-solo-DB_Login] P0/P75：ORIG 实参在 NumberToString **之后**从 -0x18 槽
    // 重载（`mov -0x18(%ebp),%edx`），无 cast 中转 ⇒ 具名局部即 unsigned int。
    unsigned int resetBase = (unsigned int)GetCurrentResetBaseTime(
        GlobalData::s_systemTime_.getCurSec(),
        reinterpret_cast<EnvView*>(G_CEnvironment())->m_dailyResetHour);
    db->set_query(
        "seLect board,reward from event_1208_bingo where m_id = %s and occ_date > from_unixtime(%u)",
        NumberToString(sig->m_accId, 0),
        resetBase);
    if (!db->exec(true))
    {
        return 0;
    }
    int rows = db->get_n_rows();             // P0：ORIG 具名槽 -0x14
    if (rows == 0)
    {
        return 1;
    }
    // P198①：ORIG 单一 bool 槽 -0xd 预清 + 独立赋值语句（fetch/get_int/get_byte 复用）。
    bool ok = false;
    ok = db->fetch();
    if (!ok)
    {
        // P227：全表达式临时链（NumberToString 入 %ebx 跨 ctor）。
        cMyTrace(__PRETTY_FUNCTION__, 0xb71b, 5)(
            "DB_Login::GetBingoEvent, fetch() ERROR m_id=%s",
            NumberToString(sig->m_accId, 0));
        return 0;
    }
    // [b28d-solo-DB_Login] ORIG 0x08448d7a..：col=0 递增计数器（col++）+ 复用同一
    // bool 槽 -0xd；get_int/get_byte 直接写 sig 字段（无 reward/board 局部）；
    // trace 传递增后的 col（P198①/P227）。
    int col = 0;
    ok = db->get_int(col++, reinterpret_cast<int&>(sig->m_bingoEventId));   // +0x413c
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xb723, 5)(
            "DB_Login::GetBingoEvent, get(%d) ERROR m_id=%s", col,
            NumberToString(sig->m_accId, 0));
        return 0;
    }
    ok = db->get_byte(col++, sig->m_bingoWinFlag);                          // +0x4138
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xb729, 5)(
            "DB_Login::GetBingoEvent, get(%d) ERROR m_id=%s", col,
            NumberToString(sig->m_accId, 0));
        return 0;
    }
    // [b28d-solo-DB_Login] 真语义修正：ORIG 成功路径落 08448eac `mov $0x0,%eax`
    // （Ghidra C `uVar3 = 0`）⇒ 返回 0（仅 rows==0 与事件未开启返回 1）。
    return 0;
}
