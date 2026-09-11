// ============================================================================
// DB_SaveExpertJob::dispatch —— ORIG 0x084367f4（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_2。GetOutBuffer<SIG_EXPERT_JOB> → upDate charac_expert_job
// （giveup_cnt=%d(ushort 零扩)、info/info_ex=%d、recipe='%s'、charac_no=%u）
// exec 失败返 0；affected==0 补 inSert；inSert exec 失败返 0；全绿返 1。
bool DB_SaveExpertJob::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_EXPERT_JOB* sig = stream->GetOutBuffer<SIG_EXPERT_JOB>();
    // register 且类型精确匹配字段（int 对 int）→ 直落 %esi（ORIG 0x039），
    // unsigned 版本或多一重 cast 会退回 scratch+copy 形态。
    {
        register int characNo = sig->m_characNo;
        db->set_query(
        "upDate charac_expert_job set expert_job_giveup_cnt=%d, expert_job_info=%d, expert_job_info_ex=%d, recipe='%s' where charac_no=%u",
        (int)sig->m_giveupCount, sig->m_endurance, sig->m_enchantCount,
        db->blob_to_str(0, sig->m_recipeFlag, 0x1e),
        characNo);
    }
    if (db->exec(true) != 1)
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_expert_job(charac_no, expert_job_giveup_cnt, expert_job_info, expert_job_info_ex, recipe) values(%u, %d, %d, %d, '%s')",
            (unsigned int)sig->m_characNo, (int)sig->m_giveupCount,
            sig->m_endurance, sig->m_enchantCount,
            db->blob_to_str(0, sig->m_recipeFlag, 0x1e));
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}