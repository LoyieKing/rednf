bool DB_LoadCharac::_UpdateSchoolCharac(SIG_LOAD_CHARAC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)0x10,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "upDate school_member set job=%d,grow_type=%d,lev=%d where "
        "school_id=%d and m_id=%s and charac_no=%d",
        (int)(char)sig->m_characData.m_field24,   // job（uchar→(char) 双扩展）
        (int)(char)sig->m_characData.m_field27,   // grow_type（uchar→(char)）
        (int)(short)sig->m_characData.m_field25,  // lev（ushort→(short) 双扩展）
        sig->m_characData.m_schoolNo,             // school_id（unsigned int 同型直载，
                                                  // ORIG `mov 0x682(%edx),%esi` 无中转）
        NumberToString(sig->m_accId, 0),          // m_id
        sig->m_characNo);                         // charac_no
    // P0/N96：清除存量 register asm("al") 绑定；exec 返回 bool ⇒ 裸形直产
    // ORIG `call; xor $1; test %al; je <mov$1 岛>`（P168a①）。
    if (!db->exec(true))
        return false;
    return true;
