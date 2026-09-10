bool DB_ChangeLetterStat::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    DB_LetterStatView* sig = reinterpret_cast<DB_LetterStatView*>(
        stream->GetOutBuffer<SIG_LETTER_STAT>());
    // ORIG 0x084260xx switch 下探链形：`movzwl; cwtl; test; je DEL;
    // test; js DEF; sub $2; cmp $1; ja DEF; [fall UPD]` —— 4.1 switch
    // {0,2,3}+default 专属；4.4 的 if/else-if 链反产三次重装载。
    switch ((int)sig->m_state)
    {
    case 0:
        db->set_query("deLete from letter where letter_id = %d",
                      sig->m_mailIdx);
        if (db->exec(true) != 1)
        {
            makeRequest(userIdx, slot, sig->m_characNo, 0, 0);
            return 0;
        }
        break;
    case 2:
    case 3:
        db->set_query("upDate letter set stat=%d where letter_id=%d",
                      (int)sig->m_state, sig->m_mailIdx);   // varargs 无 cast ⇒ 单跳
        if (db->exec(true) != 1 || db->getAffectedRowCount() == 0)
        {
            makeRequest(userIdx, slot, sig->m_characNo, 0, 0);
            return 0;
        }
        break;
    default:
        makeRequest(userIdx, slot, sig->m_characNo, 0, 0);
        return 0;
    }
    makeRequest(userIdx, slot, sig->m_characNo,
                (int)sig->m_mailIdx,   // (int)unsigned cast ⇒ 双跳
                (int)sig->m_state);
    return 1;
