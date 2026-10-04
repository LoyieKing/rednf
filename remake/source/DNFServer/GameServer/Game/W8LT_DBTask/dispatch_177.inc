// ============================================================================
// DB_InsertEventStageLog::dispatch —— ORIG 0x08449144（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(DB_HANDLE_9,0) → GetOutBuffer<SIG_EVENT_STAGE_LOG>（+0x00
// event_id、+0x04 m_id、+0x08 charac_no、+0x0c server_info、+0x10 stage、
// +0x14 stage_result）→ set_query("inSert into dnf_event_accept_list
// (event_id,m_id,charac_no,server_info,occ_date,stage,stage_result) values
// (%d,%s,%d,%d,now(),%d,'%s')", event_id, NumberToString(m_id,0), charac_no,
// server_info, stage, stage_result) → 返 exec(true)==1。
// ============================================================================
bool DB_InsertEventStageLog::dispatch(int /*userIdx*/, int /*slot*/,
                                      Stream* stream)
{
    const char* stageResult;   // -0x2c（ORIG：GetOutBuffer 后 add $0x14 缓存）
    // 技巧31 dead 槽（ORIG 0x08449181(-0x2c) 与 db(-0x20) 间存在 4.1 遗留
    // 空洞 -0x24/-0x28；4.4 槽压紧需显式补位翻 ORIG 槽号）。
    unsigned int gapA;
    unsigned int gapB;
    MySQL* db;                 // -0x20
    SIG_EVENT_STAGE_LOG* sig; // -0x1c
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    sig = (
        stream->GetOutBuffer<SIG_EVENT_STAGE_LOG>());
    stageResult = sig->m_stageResult;
    (void)gapA;
    (void)gapB;
    db->set_query(
        "inSert into dnf_event_accept_list(event_id,m_id,charac_no,server_info,occ_date,stage,stage_result) values(%d,%s,%d,%d,now(),%d,'%s')",
        sig->m_eventId, NumberToString(sig->m_mId, 0), sig->m_characNo,
        sig->m_serverInfo, sig->m_stage, stageResult);
    if (db->exec(true) != 1)
        return 0;
    return 1;
}