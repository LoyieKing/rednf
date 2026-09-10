bool DB_UpdateComboSkill::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3,
                                                  E_SERVER_GROUP_NONE);
    if (db == 0)
        return 0;
    SIG_COMBO_SKILL* sig = stream->GetOutBuffer<SIG_COMBO_SKILL>();
    if (sig == 0)
        return 0;
    if (!setComboSkill(db, (unsigned int)sig->m_characNo, sig->m_tree1Count,
                       sig->m_tree1Data, SKILL_TREE_1))
    {
        // b28c-L19 清绑（P0/N96）：改全表达式临时链内联实参（P227），
        // 由 4.4 自然升格 callee-saved 保活 characNo 跨 ctor call。
        cMyTrace(__PRETTY_FUNCTION__, 0xbb2c, 5)(
            "DB_UpdateComboSkill::dispatch, setComboSkill ERROR charac_no=%u",
            (unsigned int)sig->m_characNo);
    }
    if (!setComboSkill(db, (unsigned int)sig->m_characNo, sig->m_tree2Count,
                       sig->m_tree2Data, SKILL_TREE_2))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xbb33, 5)(
            "DB_UpdateComboSkill::dispatch, setComboSkill ERROR charac_no=%u",
            (unsigned int)sig->m_characNo);
    }
    return 1;
