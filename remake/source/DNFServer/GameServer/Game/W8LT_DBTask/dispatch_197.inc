// [b28d-4] DIFF：N18a/P211：4.4 多 {'push': 1, 'mov': 6, 'test': 2, 'je': 2, 'xor': 2, 'jmp': 2, 'movb': 4, 'movzbl': 2, 'lea': 1, 'rep': 1, 'pop': 1} 中转 mov（单段直落/同型化待验）；ORIG 08423bc4
bool DB_PassPadUpdateFailCnt::dispatch(int /*userIdx*/, int /*slot*/,
                                       Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_SECURITY_CARD_UPDATE_FAIL_CNT* sig =
        stream->GetOutBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>();
    // [FL3W8LT 2026-09-10 P183/P328①] 原具名视图局部
    // `DB_PassPadUpdateFailCntView* v = reinterpret_cast<...>(sig);` 已删：
    // ORIG 0x0843bc4 段每处字段访问都重发 `mov -0x10(%ebp),%eax;
    // mov (%eax),%eax`（重读 sig 槽再解引用，无独立视图槽），且帧
    // ORIG `push %ebx; sub $0x94` vs 我方 `push %edi; sub $0xa0` ⇒ 每点内联
    // 视图表达式（P89 单一视图表达式）。
#define PSIG ((DB_PassPadUpdateFailCntView*)sig)
    db->set_query(
        "upDate member_mousepass set fail_cnt=%d where m_id=%s and enable_flag = '1'",
        PSIG->m_failCnt, NumberToString(PSIG->m_mId, 0));
    db->exec(true);
    if (PSIG->m_failCnt != 0)
    {
        db->set_query(
            "seLect mousepass from member_mousepass where m_id=%s",
            NumberToString(PSIG->m_mId, 0));
        // [H4 2026-09-11] ORIG 0x08423c92..0x08423cc8 实证：bool 值**不落栈槽** ——
        // `xor $1,%eax; test %al,%al; jne <fail>` + `test %eax,%eax; je <fail>` +
        // `xor $1,%eax; test %al,%al; je <succ>` 短路链，`fail: mov $1,%eax` /
        // `succ: mov $0,%eax` 汇聚后 `test %al,%al; je` 直判 ⇒ 原码**无具名 bool
        // 局部**（P0 槽-局部双向对应律），直接判复合条件（旧「true 初值 + 置 false
        // + 正判」形多 4 movb + 2 movzbl 槽往返）。
        if (!(db->exec(true) == 1 && db->get_n_rows() != 0 && db->fetch() == 1))
            return 0;
        // [H4 2026-09-11] ORIG 0x08423cd4 直接 `lea -0x48(%ebp),%eax; …call get_str`
        // —— **无**预清（原码 `char mousepass[0x24];`，P171/P328⑤ 不加多余初始化；
        // 失败路径已 return 0，无未初始化读取）。
        char mousepass[0x24];           // -0x48
        if (db->get_str(0, mousepass, 0x24) != 1)
            return 0;
        char ym[10] = { 0 };            // -0x1e（strftime "%Y%m"）
        time_t curSec = GlobalData::s_systemTime_.getCurSec();   // -0x24
        struct tm tmBuf;                // -0x74
        localtime_r(&curSec, &tmBuf);
        strftime(ym, 10, "%Y%m", &tmBuf);
        db->set_query(
            "inSert into member_mousepass_history_%s (occ_time, m_id, pre_mousepass, modify_type, ip_info, port_info) values (now(),%s,'%s',%d,'%s','')",
            ym, NumberToString(PSIG->m_mId, 0), mousepass, 0x11, PSIG->m_ipInfo);
        db->exec(true);
    }
    MySQL* dbGrade = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                       E_SERVER_GROUP_NONE);
    dbGrade->set_query(
        "upDate member_security_grade set goblin_fail_cnt=%d where m_id=%s",
        PSIG->m_failCnt, NumberToString(PSIG->m_mId, 0));
    // [H4 2026-09-11] 同上（P0）：ORIG 下部 bool 同走 `mov $1/$0,%eax` 寄存器物化
    // ⇒ 无具名局部，直接判复合条件。
    if (!(dbGrade->exec(true) == 1 && dbGrade->getAffectedRowCount() != 0))
    {
        dbGrade->set_query(
            "inSert into member_security_grade(m_id, goblin_fail_cnt) values(%s, %d)",
            NumberToString(PSIG->m_mId, 0), PSIG->m_failCnt);
        if (dbGrade->exec(true) != 1)
            return 0;
    }
#undef PSIG
    return 1;
}