// NEAR 豁免：指令全同（125=125），仅 db/r 栈槽差 4 字节（ORIG gcc4.1 布局
// db@-0x1c/r@-0x1d；本工具链 gcc4.4 固定 db@-0x20/r@-0x19，6 种声明序穷举
// 均无法复现，属跨编译器栈布局差异，语义等价）。
bool DB_SaveSkill::SaveSkill(SIG_LOAD_SKILL* sig)
{
    static const int handleIdx = 3;  // ORIG _ZZN12DB_SaveSkill9SaveSkill…E9handleIdx
    // 声明序 out1→out2→out3→r→db 对应 ORIG 栈槽 -0x24/-0x28/-0x2c/-0x1d/-0x1c
    // （4.4.7 -O0 同对齐组内首声明者靠高；证据 0x08417850/853/084179e6）。
    char* out1;
    char* out2;
    char* out3;
    bool r;
    MySQL* db;
    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    out1 = 0;
    if (!put_compressed_blob_data(db, 0, (char*)sig->m_skillSlotTree1, 0x198, &out1))
        return 0;
    out2 = 0;
    if (!put_compressed_blob_data(db, 1, (char*)sig->m_skillSlotTree2, 0x198, &out2))
        return 0;
    out3 = 0;
    if (!put_compressed_blob_data(db, 4, (char*)sig->m_commands, 0xc8, &out3))
        return 0;
    db->set_query(
        "upDate %s set remain_sp=%u,skill_slot='%s',remain_sp_2nd=%u, skill_slot_2nd='%s', remain_sfp_1st=%u, remain_sfp_2nd=%u, skill_command ='%s',script_version=%d where charac_no=%u",
        get_skill_table_name(sig->m_flag410),
        sig->m_remainSpTree1,
        out1,
        sig->m_remainSpTree2,
        out2,
        sig->m_remainSfpTree1,
        sig->m_remainSfpTree2,
        out3,
        sig->m_field411,
        sig->m_characNo);
    r = db->exec(true);
    if (!r)
        return 0;
    return 1;
}