// [b28d-4] DIFF：N18a/P211：4.4 多 {'mov': 2} 中转 mov（单段直落/同型化待验）；ORIG 0842c362
bool DB_SecurityCardRetransfer::dispatch(int userIdx, int slot,
                                         Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;

    SIG_SECURITY_CARD_RETRANSFER* sig = (
        stream->GetOutBuffer<SIG_SECURITY_CARD_RETRANSFER>());
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect re_issue_cnt,unix_timestamp(last_issue_time), phone from member_security_card where m_id=%s",
        NumberToString(sig->m_mId, 0));
    try
    {
        if (!db->exec(true))
            throw (ENUM_ERROR)1;
        if (!db->fetch())
            throw (ENUM_ERROR)1;
        // P171/P328⑤：ORIG 无预清（两值随后由 get_uint 写入）。
        // [H4 2026-09-11 复核] ORIG 0x0842c43e/0x0842c445 虽有 `movl $0x0,
        // -0x1c/-0x20(%ebp)`，但给本两值加 `= 0` 实测**反劣**（ours 272→274、
        // 差异行 93→188）⇒ 那两条清零属其它语义（非本两值声明初始化）⇒ 维持无预清。
        unsigned int reIssueCnt;
        unsigned int lastIssueTime;
        db->get_uint(0, reIssueCnt);
        db->get_uint(1, lastIssueTime);
        char phone[0x29];
        memset(phone, 0, sizeof(phone));
        db->get_str(2, phone, 0x29);
        if (strncmp(sig->m_phone, phone, 0x29) != 0)
            throw (ENUM_ERROR)0x7d;

        // [H4 2026-09-11 复核] 试删 `time_t cur` 副本（改用 &curSec 直传）实测**反劣**
        // （ours 272→270、差异行 93→211）⇒ 维持 `time_t cur` 具名副本（4.4 槽分配联动）。
        int curSec = GlobalData::s_systemTime_.getCurSec();
        time_t last = (time_t)lastIssueTime;
        time_t cur = (time_t)curSec;
        struct tm tLast;
        struct tm tCur;
        localtime_r(&last, &tLast);
        localtime_r(&cur, &tCur);
        if (tLast.tm_yday == tCur.tm_yday)
        {
            reIssueCnt += 1;
            if (reIssueCnt > 3)
                throw (ENUM_ERROR)0x78;
        }
        else
        {
            reIssueCnt = 0;
        }

        // [H4 2026-09-11 语义轴·照抄] ORIG 0x0842c5ed..0x0842c60f 实证实参序为
        //   `lea 0x2b(%ecx),%esi; lea 0x4(%ecx),%ebx; add $0x25,%ecx` →
        //   `mov %ecx,0x8(%esp)`（第1个 %s=cert_key）/ `mov %ebx,0xc(%esp)`
        //   （server_key）/ `mov %esi,0x10(%esp)`（card）
        // ⇒ ORIG 传 (+0x25, +0x4, +0x2b)，即 m_card/m_certKey/m_serverKey 的次序
        //（与列名字面语义错位，Ghidra C 亦为 `local_1c+0x25, local_1c+4,
        //  local_1c+0x2b`）——按 P171 原码行为照抄原则复现，勿"按语义纠正"。
        // 同函数 `_sendResult(slot, sig+0x52, sig+0x4, sig+0x2b, 0)` 保持不变。
        db->set_query(
            "upDate member_security_card set cert_key='%s',server_key='%s',card='%s',fail_cnt=0,cancel_cnt=0,re_issue_cnt=%d,last_issue_time=now(),validity_time=0,apply_flag=1,cert_flag=0,cert_cnt=0 where m_id=%s",
            sig->m_card, sig->m_certKey, sig->m_serverKey, reIssueCnt,
            NumberToString(sig->m_mId, 0));
        db->exec(true);
        _sendResult(slot, sig->m_serial, sig->m_certKey, sig->m_serverKey,
                    (ENUM_ERROR)0);
    }
    // [2026-09-11 真 bug 修复] ORIG 4 处 throw 的 typeinfo 均为 `_ZTI10ENUM_ERROR`
    // （0x0842c42e / 0x0842c47a / 0x0842c538 / 0x0842c5be 均 `movl $0x8b75f40,0x4(%esp)`，
    //  nm: _ZTI10ENUM_ERROR @0x08b75f40）；我方原为 4×`_ZTIi`（throw int + catch(int)）
    //  ⇒ 异常类型错、跨模块不可接。按 ORIG 改为 throw/catch (ENUM_ERROR)。
    catch (ENUM_ERROR err)
    {
        _sendResult(slot, 0, 0, 0, err);
    }

    MySQL* dbGrade = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                       E_SERVER_GROUP_NONE);
    dbGrade->set_query(
        "upDate member_security_grade set security_card_reg=now() where m_id=%s",
        NumberToString(sig->m_mId, 0));
    if (!(dbGrade->exec(true) && dbGrade->getAffectedRowCount() != 0))
    {
        dbGrade->set_query(
            "inSert into member_security_grade(m_id, security_card_reg) values(%s, now())",
            NumberToString(sig->m_mId, 0));
        if (!dbGrade->exec(true))
            return 0;
    }
    return 1;
}