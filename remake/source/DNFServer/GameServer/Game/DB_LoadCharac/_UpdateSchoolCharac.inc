// ============================================================================
// DB_LoadCharac::_UpdateSchoolCharac @0x08408c30
// school_member 更新：job(+0x30)/grow_type(+0x33)/lev(+0x31)，
// where school_id=+0x682 and m_id=NumberToString(accId,0) and charac_no=+0x4。
// 返回 exec(true) 结果。
// 报告 08408c6f..08408c98 实证读取形态：movzbl+movsbl（unsigned char lvalue 经
// (char) 转换）与 movzwl+movswl（unsigned short lvalue 经 (short) 转换）→ 绑定
// **旧类型占位字段**（非 char/short 别名，别名绑 char& 供 _getCharacInfo 用）。
// ============================================================================
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
}