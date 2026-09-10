bool DB_LoadServerMessage::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    DB_ServerMessageSelectView* sel =
        reinterpret_cast<DB_ServerMessageSelectView*>(
            stream->GetOutBuffer<SIG_SELECT_SERVER_MESSAGE>());
    db->set_query("seLect kind, message_index, charac_name, message, unix_timestamp(update_time) from event_server_message where server_info=%d and (channel_no=%d or channel_no=0)",
                  (unsigned int)(unsigned short)sel->m_server,
                  (unsigned int)(unsigned short)sel->m_channel);
    if (db->exec(true) != 1)
        return 0;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8131), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    // ORIG：call operator->（返回 this）后再 GetInBuffer（0x80c8c4e 形态）。
    SIG_EVENT_SERVER_MESSAGE* out =
        guard.operator->()->GetInBuffer<SIG_EVENT_SERVER_MESSAGE>();
    memset(out, 0, 0x1134);
    int nRows = db->get_n_rows();
    // 清零序对齐 ORIG：ok → kind → msgIdx → message(rep stos) →
    // characName(movl×5) → timestamp。
    bool ok = false;
    // [b28d-solo-W8LT_DBTask] P266 反向：ORIG store 前 `movzbl -0x3d,%eax; mov %eax,%ecx`
    // 两段装载 ⇒ 源是 char（uchar 源 4.4 恒折单段 `movzbl -0x3d,%ecx`）。
    char kind = 0;
    char msgIdx = 0;
    char message[0x32] = { '\0' };
    char characName[0x14] = { '\0' };
    int timestamp = 0;
    int rowIdx = 0;
    for (int i = 0; i < nRows; ++i)
    {
        ok = db->fetch();
        if (!ok)
            return 0;
        ok = db->get_byte(0, (char&)kind);
        if (!ok)
            return 0;
        ok = db->get_byte(1, (char&)msgIdx);
        if (!ok)
            return 0;
        ok = db->get_str(2, characName, 0x14);
        if (!ok)
            return 0;
        ok = db->get_str(3, message, 0x32);
        if (!ok)
            return 0;
        ok = db->get_int(4, timestamp);
        if (!ok)
            return 0;
        // ORIG 每字段独立重算 out + rowIdx*0x58（P36：不命名行指针缓存）。
        // 残留 ndiff=2：ORIG 0x843619x 值装载为两步 `movzbl slot,%eax; mov
        // %eax,%ecx`（4.1 值寄存器再拷入 store 数据槽），4.4 恒折单步
        // `movzbl slot,%ecx`；register asm 探针 /tmp/w8lt_bs2.o::f4 反致
        // 装载序/寻址全乱 ⇒ N18a 类跨版本折叠，标 blocked。
        out->m_msg[rowIdx].m_kind = kind;
        out->m_msg[rowIdx].m_index = msgIdx;
        out->m_msg[rowIdx].m_lenMsg = (int)strlen(message);
        strncpy(out->m_msg[rowIdx].m_msg, message, strlen(message));
        out->m_msg[rowIdx].m_lenMsg2 = (int)strlen(characName);
        strncpy(out->m_msg[rowIdx].m_msg2, characName, strlen(characName));
        out->m_msg[rowIdx].m_endTime = timestamp;
        ++rowIdx;
    }
    out->m_count = (unsigned char)nRows;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
