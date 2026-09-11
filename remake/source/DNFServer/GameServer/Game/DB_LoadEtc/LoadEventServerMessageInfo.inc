// ============================================================================
// DB_LoadEtc::LoadEventServerMessageInfo @0x084356a2 —— event_server_message
// 按服务器/频道装载进 SIG_LOAD_ETC 条目数组（base +0x2d38 起，stride 0x58）。
// entry 布局（ORIG 写点）：+0x00 kind(byte) | +0x01 index(byte) |
//   +0x08 strlen(size_t) | +0x0c message[50 内] | +0x40 strlen | +0x44 name[20]
//   | +0x58 update_time(int) —— 以文档化偏移直写，条目计数入 +0x2d38。
// ============================================================================
bool DB_LoadEtc::LoadEventServerMessageInfo(SIG_LOAD_ETC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)6,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect kind, message_index, charac_name, message,"
                  " unix_timestamp(update_time) from event_server_message where"
                  " server_info=%d and (channel_no=%d or channel_no=0)",
                  sig->m_serverGroup /*+0x2d30*/, sig->m_channelNo /*+0x2d34*/);
    if (!db->exec(true))
    {
        return false;
    }

    int nRows = db->get_n_rows();                            // local_18
    bool ok = false;                                         // local_11 复用结果槽
    char kind = 0;                                           // local_1d
    char msgIndex = 0;                                       // local_1e
    // ORIG 89045735.. 两段展开初始化：message[0x32] 走 2B头+rep stos+2B/1B尾
    // （= 大数组 {0} 初始化形），name[0x14] 走 5 条 movl（小数组）。
    char message[50] = { 0 };                                // local_6a
    char name[20] = { 0 };                                   // local_32
    int updateTime = 0;                                      // local_38
    int written = 0;                                         // -0x10（entry 索引）

    for (int i = 0; i < nRows; ++i)                          // -0xc（循环计数）
    {
        ok = db->fetch();
        if (!ok)
        {
            return false;
        }
        ok = db->get_byte(0, kind);                          // col0 kind
        if (!ok)
        {
            return false;
        }
        ok = db->get_byte(1, msgIndex);                      // col1 message_index
        if (!ok)
        {
            return false;
        }
        ok = db->get_str(2, name, 0x14);                     // col2 charac_name
        if (!ok)
        {
            return false;
        }
        ok = db->get_str(3, message, 0x32);                  // col3 message
        if (!ok)
        {
            return false;
        }
        ok = db->get_int(4, updateTime);                     // col4 update_time
        if (!ok)
        {
            return false;
        }

        // ORIG 89058e6..9cb：每个写点独立重算 sig + written*0x58 + base（P157 逐字段
        // operator[] 重读，不缓存 entry 指针）；strlen 结果直存 + strncpy 限长拷贝。
        sig->m_eventMessages[written].m_kind = (unsigned char)kind;       // +0x00
        sig->m_eventMessages[written].m_index = (unsigned char)msgIndex;  // +0x01
        sig->m_eventMessages[written].m_messageLen = strlen(message);     // +0x04
        // ORIG +0x2bf/+0x30f 用 strncpy(dst,src,strlen(src))，且 n 独立重调
        // strlen（P48 去缓存，不复用 m_messageLen）。
        strncpy(sig->m_eventMessages[written].m_message, message,
                strlen(message));                                        // +0x08
        sig->m_eventMessages[written].m_nameLen = strlen(name);           // +0x3c
        strncpy(sig->m_eventMessages[written].m_name, name,
                strlen(name));                                            // +0x40
        sig->m_eventMessages[written].m_updateTime = updateTime;          // +0x54
        ++written;
    }

    sig->m_eventMessageCount = (unsigned char)nRows;     // +0x2d38 movb 截断
    return true;
}