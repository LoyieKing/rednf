// ============================================================================
// DB_SecurityCardIssue::dispatch —— ORIG @ 0x0842af94（0x47c）：
// 基类 DBDispatcher::dispatch 放行（失败返 0）→ GetOutBuffer<
// SIG_SECURITY_CARD_ISSUE> → GetDBHandle(DB_HANDLE_6)。try：
//   查询 phone apply_flag：exec 失败或行数>4 → throw (ENUM_ERROR)0x77；
//   查 re_issue_cnt/last_issue：exec 失败 → throw 1；
//   char q[5] 清零 → generateSecurityCardQuestion(q) → strncpy(sig+0x5e,q,5)；
//   无行 → insert（exec 失败 throw 1）；有行 → fetch 失败 throw 1；
//     get_uint(0,nIssue) / get_uint(1,lastTime) → now=getCurSec() →
//     localtime_r 两次比较 tm_yday：同日 ++nIssue 超 3 → throw 0x78，
//     异日归零 → update（exec 结果丢弃）；
//   turnOffSecurityFlag(db, m_id) → _sendResult(this, slot, sig+0x25,
//     sig+0x04, sig+0x37, sig+0x5e, 0)。
// catch (ENUM_ERROR& e) → _sendResult(this, slot, 0,0,0,0, e)。返 1。
// ============================================================================
// [b28d-54 修复] ORIG 0x0842af94：唯一残差为 localtime_r 两实参求值序
//   （ORIG arg2 先）；`unsigned int now` + reinterpret_cast 使 4.4 arg1 先。
//   改 `time_t now` + 纯 `&now` 后与 ORIG 同形（289=289，AE）。
bool DB_SecurityCardIssue::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;

    SIG_SECURITY_CARD_ISSUE* sig =
        stream->GetOutBuffer<SIG_SECURITY_CARD_ISSUE>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    try
    {
        db->set_query(
            "seLect apply_flag from member_security_card where phone='%s' "
            "and apply_flag=1",
            &sig->m_phone[0]);
        if (!db->exec(true) || (unsigned int)db->get_n_rows() > 4)
            throw static_cast<ENUM_ERROR>(0x77);

        db->set_query(
            "seLect re_issue_cnt,unix_timestamp(last_issue_time) from "
            "member_security_card where m_id=%s",
            NumberToString(sig->m_mId, 0));
        if (!db->exec(true))
            throw static_cast<ENUM_ERROR>(1);

        char q[5] = { '\0' };
        generateSecurityCardQuestion(q);
        strncpy(&sig->m_qPos[0], q, 5);

        if (db->get_n_rows() == 0)
        {
            db->set_query(
                "inSert into member_security_card(m_id,occ_time,phone,"
                "cert_key,server_key,card,fail_cnt,re_issue_cnt,"
                "last_issue_time,validity_time,apply_flag,q_pos) "
                "values(%s,now(),'%s','%s','%s','%s',0,0,now(),0,0,'%s')",
                NumberToString(sig->m_mId, 0), &sig->m_phone[0],
                &sig->m_certKey[0], &sig->m_serverKey[0], &sig->m_card[0], q);
            db->exec(true);   // ORIG 无检查（结果丢弃）
        }
        else
        {
            unsigned int nIssue = 0;   // ORIG 声明/初始化先于 fetch
            unsigned int lastTime = 0;
            if (!db->fetch())
                throw static_cast<ENUM_ERROR>(1);
            db->get_uint(0, nIssue);
            db->get_uint(1, lastTime);

            // b28d-54：now 仅用于 localtime_r；声明为 time_t 使实参为纯 `&now`
            //   （与第一次调用同形 ⇒ 4.4 复现 ORIG arg2 先求值序，P23）。
            time_t now =
                static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
            time_t tLast = static_cast<time_t>(lastTime);
            struct tm tmLast;
            struct tm tmNow;
            localtime_r(&tLast, &tmLast);
            localtime_r(&now, &tmNow);

            if (tmLast.tm_yday == tmNow.tm_yday)
            {
                ++nIssue;
                if (nIssue > 3)
                    throw static_cast<ENUM_ERROR>(0x78);
            }
            else
            {
                nIssue = 0;
            }

            db->set_query(
                "upDate member_security_card set phone='%s',cert_key='%s',"
                "server_key='%s',card='%s',fail_cnt=0,re_issue_cnt=%d,"
                "last_issue_time=now(),validity_time=0,apply_flag=0,"
                "q_pos='%s' where m_id=%s",
                &sig->m_phone[0], &sig->m_certKey[0], &sig->m_serverKey[0],
                &sig->m_card[0],
                nIssue, q, NumberToString(sig->m_mId, 0));
            db->exec(true);
        }

        turnOffSecurityFlag(db, sig->m_mId);
        _sendResult(slot, &sig->m_serial[0], &sig->m_serverKey[0],
                    &sig->m_card[0], &sig->m_qPos[0], ENUM_ERROR_NONE);
    }
    // [b28c-L29] ORIG 0x0842af94：begin_catch 后直接 mov (%eax),%eax（值先
    // 入槽，无指针槽回读）⇒ 按值 catch（P229）。
    catch (ENUM_ERROR e)
    {
        _sendResult(slot, 0, 0, 0, 0, e);
    }
    return 1;
}