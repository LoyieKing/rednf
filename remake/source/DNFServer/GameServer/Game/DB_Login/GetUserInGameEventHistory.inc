// [S216_DB_Login2] verdict=AE ndiff=0 gap=0（0x084152b0 119=119；下列 N1 判词作废）
// `add $0x3fa0; add 0xc(%ebp); add $0xc` 三步，4.4 恒折 $0x3fac + lea；十形探针 +
// P311 二维视图思路均不翻）。
// S216 收口（原 N1 判词作废）：P235/P311 历史表数组视角（SigTypes_D.h 匿名 union
// m_ingameEventHistoryView）⇒ 4.4 复现 `add $0x3fa0; add sig; add $0xc` 三步链，
// 逐条 = ORIG（探针 /tmp/s216_probe3.cpp fH）。
int DB_Login::GetUserInGameEventHistory(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    memset(SGBC(sig, OFF_HISTORY_TBL), 0, 0x168);    // 30 行 × {type,h1,h2}
    db->set_query(
        "seLect type , history_1 ,history_2 from event_ingame_history where m_id=%s order by type desc",
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    int nRows = db->get_n_rows();
    bool ok;
    for (int i = 0; i < nRows; ++i)
    {
        if (nRows > 0x1d)
        {
            return 1;                    // ORIG 上限即断（>29 行不再读取）
        }
        ok = db->fetch();
        if (!ok)
        {
            break;
        }
        // [S216/P235] 改具名历史表数组视角（SigTypes_D.h 匿名 union）⇒ 4.4 产
        // `imul/三 add 链 + add $0x3fa0 + add sig + add $0xc` 逐条 = ORIG（探针
        // /tmp/s216_probe3.cpp fH）；旧裸 char* 算术恒折 $0x3fac（N1）。
        ok = db->get_int(0, sig->m_ingameEventHistoryView.m_rows[i].m_type);
        if (!ok)
        {
            break;
        }
        ok = db->get_int(1, sig->m_ingameEventHistoryView.m_rows[i].m_history1);
        if (!ok)
        {
            break;
        }
        ok = db->get_int(2, sig->m_ingameEventHistoryView.m_rows[i].m_history2);
        if (!ok)
        {
            break;
        }
    }
    return 1;
}