// ORIG 0x0843712e：GetDBHandle(DB_HANDLE_1,0) → GetOutBuffer<
// SIG_RESTING_USER_RESTRICT> → set_query("upDate member_punish_info set
// apply_flag = 0 where m_id = %s and punish_type = 10", NumberToString(mid,0))
// → exec 失败返 0；成功 Acquire("DBThread.cpp",0x835c) → guard <<0x195
// <<stream(int) → put(DISPATCH_Q=1,guard)；再 GetDBHandle(DB_HANDLE_1,0)：
// char reason[0xff]={0}（P4）→ memcpy(reason, EUC-KR
// "|*|휴면 제재 해제|*|보안팀", 0x1b)（DAT_08c53b62，27 字节含 NUL）→
// time_t curSec = s_systemTime_.getCurSec() → localtime_r(&curSec,&tm) →
// strftime(year,10,"%Y",&tm) → set_query("upDate
// member_punish_info_history_%s set apply_flag = 0 , reason = '%s' where
// m_id = %s and punish_type = %d and apply_flag = 1", year, reason,
// NumberToString(mid,0), 10) → ok=exec(true)；ok 时 cMyTrace(0x8381,0)
// "CANCEL OK - Resting User Restrict (mid:%s)"；返 ok。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0843712e
bool DB_CancelRestingUserRestrict::dispatch(int userIdx, int slot,
                                            Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    SIG_RESTING_USER_RESTRICT* sig =
        stream->GetOutBuffer<SIG_RESTING_USER_RESTRICT>();
    db->set_query(
        "upDate member_punish_info set apply_flag = 0 where m_id = %s and punish_type = 10",
        NumberToString(sig->m_uid, 0));
    if (!db->exec(true))
        return 0;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x835c), true);
    **guard << 0x195;
    **guard << slot;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    MySQL* db2 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    char reason[0xff] = { '\0' };
    memcpy(reason,
           "|*|\xC8\xDE\xB8\xE9 \xC1\xA6\xC0\xE7 \xC7\xD8\xC1\xA6|*|\xBA\xB8\xBE\xC8\xC6\xC0",
           0x1b);
    char year[10] = { '\0' };
    time_t curSec = (time_t)GlobalData::s_systemTime_.getCurSec();
    struct tm tmNow;
    localtime_r(&curSec, &tmNow);
    strftime(year, 10, "%Y", &tmNow);
    db2->set_query(
        "upDate member_punish_info_history_%s set apply_flag = 0 , reason = '%s' where m_id = %s and punish_type = %d and apply_flag = 1",
        year, reason, NumberToString(sig->m_uid, 0), 10);
    // ORIG 0x08437380：`xor $1; test %al; je <trace>` + `mov $0/%1,%ebx` 常数岛，
    // 值跨 guard dtor 留 %ebx（尾 `mov %ebx,%eax`）⇒ 早返形（P16/P59）；
    // trace 槽 -0x34（浅于 guard -0x3c）⇒ P221 全表达式临时链。
    if (!db2->exec(true))
        return false;
    cMyTrace(__PRETTY_FUNCTION__, 0x8381, 0)(
        "CANCEL OK - Resting User Restrict (mid:%s)",
        NumberToString(sig->m_uid, 0));
    return true;
}