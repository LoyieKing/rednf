// ============================================================================
// DB_InsertPoliceSaveChatting::dispatch —— ORIG 0x0843c514
// >>characNo(u32) >>msgType(char) >>message(char* 读入 0x100 缓冲) →
// GetDBHandle(DB_HANDLE_4,0) → escape_string(message) →
// inSert log_police_chatting(charac_no, msg_type, occ_time, message)
// values(%u,%d,unix_timestamp(now()),'%s')。exec 结果丢弃，恒返 1。
// ============================================================================
// [b28d-solo-3] AE：message[0x100]/escapeBuf[0x200] 改 `={'\0'}` 并连排声明于
// 读取前（P312/P202① 复现 ORIG 两段对齐剥离慢路，0x100/0x200）。ORIG 0843c514。
bool DB_InsertPoliceSaveChatting::dispatch(int userIdx, int slot,
                                           Stream* stream)
{
    unsigned int characNo = 0;
    char msgType = 0;
    // ORIG 0x0843c514：message(-0x111,0x100)/escapeBuf(-0x311,0x200) 连续声明，
    // 两个清零都在读取前（`= {'\0'}` 复现 4.1 对齐剥离慢路，P312/P202①）。
    char message[0x100] = { '\0' };
    char escapeBuf[0x200] = { '\0' };
    *stream >> characNo;
    *stream >> msgType;
    *stream >> message;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    db->escape_string(escapeBuf, message);
    db->set_query(
        "inSert into log_police_chatting(charac_no, msg_type, occ_time, message) values(%u, %d, unix_timestamp(now()), '%s')",
        characNo, (int)msgType, escapeBuf);
    db->exec(true);
    return 1;
}