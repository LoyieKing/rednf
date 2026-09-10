bool DB_UpdateBingoEvent::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;
    int board = 0;
    char reward = 0;
    *stream >> mId;
    *stream >> board;
    *stream >> reward;
    // ORIG +0x070-0x08e：G_CEnvironment()+0x37c 折叠单指令取数（P1），
    // 用具名成员 m_serverEnvir.m_fatigueTime（0x198+0x1e4 == 0x37c）。
    // ORIG +0x92 call 后 `mov %eax,-0xc(%ebp)` 全字单槽（unsigned 视图）；
    // +0xd2 `mov -0xc(%ebp),%ebx` 迟装载紧贴 5 参 spill（0x14(%esp)）⇒
    // 无 (unsigned int) 强转的具名 unsigned 槽直读。
    unsigned int resetBase = (unsigned int)GetCurrentResetBaseTime(
        (int)GlobalData::s_systemTime_.getCurSec(),
        G_CEnvironment()->m_serverEnvir.m_fatigueTime);
    db->set_query(
        "upDate event_1208_bingo set board = %d,reward=%d where m_id = %s and occ_date > from_unixtime(%u)",
        board, (int)reward, NumberToString(mId, 0), resetBase);
    if (db->exec(true) != 1)
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into event_1208_bingo(m_id,occ_date,board,reward) values(%s,now(),%d,%d)",
            NumberToString(mId, 0), board, (int)reward);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
