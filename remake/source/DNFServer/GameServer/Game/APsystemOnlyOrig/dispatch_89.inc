// ============================================================================
// DB_UpdateServerMessage::dispatch —— ORIG 0x08435a02
// ============================================================================
// ORIG 0x8435a02：GetDBHandle(DB_HANDLE_6,0) → GetOutBuffer<
// SIG_UPDATE_SERVER_MESSAGE>（+0x10 message/+0x48 charac_name 先 escape_string
// 到 0x80/0x40 缓冲）→ upDate event_server_message（message/charac_name/
// update_time where server_info/channel_no/message_index）；exec 失败或
// affected==0 → inSert 7 占位；仍失败返 0。
// 成功后 Acquire("DBThread.cpp",0x80e3) → CStreamGuard(stream,true) →
// <<userIdx → <<slot → GetInBuffer<SIG_UPDATE_SERVER_MESSAGE>() 回包：
// memset 0x60 + 逐字段拷贝（server_info/channel_no/message_index/kind/
// charac_name_len/strncpy(charac_name)/message_len/strncpy(message)/
// update_time）→ MsgQueueMgr::put(DISPATCH_Q(1), guard) → 返 1。
// [b28d-4] DIFF：N1/N107：ORIG lea+add 两步寻址 vs 4.4 折单条；ORIG 08435a02
// [b28d-solo-4] §6.2 三件套：p167 CLEAR（ORIG 24 = OUR 24，0/0）+ gap 0（215/215）
//   ⇒ 语义完成；余 N4/N107 lea-add 折叠（P30/P319 已试，含字段/RHS 同型化）封顶。
bool DB_UpdateServerMessage::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_UPDATE_SERVER_MESSAGE* sig =
        /*SIG_UPDATE_SERVER_MESSAGE*/(
            stream->GetOutBuffer<SIG_UPDATE_SERVER_MESSAGE>());
    char msg[0x80];    // -0xdc..-0x5d（message escape 目标）
    char name[0x40];   // -0x5c..-0x1d（charac_name escape 目标）
    db->escape_string(msg, sig->m_message);
    db->escape_string(name, sig->m_characName);
    db->set_query(
        "upDate event_server_message set message='%s', charac_name='%s', update_time=from_unixtime(%d) where server_info=%d and channel_no=%d and message_index=%d",
        msg, name, sig->m_updateTime, sig->m_serverInfo, sig->m_channelNo,
        sig->m_messageIndex);
    if (!db->exec(true) || db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into event_server_message(server_info, channel_no, kind, message_index, charac_name, message, update_time) values(%d, %d, %d, %d, '%s', '%s', from_unixtime(%d))",
            sig->m_serverInfo, sig->m_channelNo, sig->m_kind,
            sig->m_messageIndex, name, msg, sig->m_updateTime);
        if (db->exec(true) != 1)
            return 0;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x80e3), true);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((int)slot);
    SIG_UPDATE_SERVER_MESSAGE* out =
        /*SIG_UPDATE_SERVER_MESSAGE*/(
            guard->GetInBuffer<SIG_UPDATE_SERVER_MESSAGE>());
    memset(out, 0, 0x60);
    out->m_serverInfo = sig->m_serverInfo;
    out->m_channelNo = sig->m_channelNo;
    out->m_messageIndex = sig->m_messageIndex;
    out->m_kind = sig->m_kind;
    out->m_characNameLen = sig->m_characNameLen;
    // 残留（N 类）：ORIG 0x8435… `lea 0x1(%eax),%ecx` 独立输出寄存器
    // 物化；4.4 恒 `add $0x1,%eax` 原地复用（register asm("ecx"/"eax")
    // /具名槽探针全试，均反产 spill，probe t1-t5 实证）。
    strncpy(out->m_characName, sig->m_characName, sig->m_characNameLen + 1);
    out->m_messageLen = sig->m_messageLen;
    strncpy(out->m_message, sig->m_message, sig->m_messageLen + 1);
    out->m_updateTime = sig->m_updateTime;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}