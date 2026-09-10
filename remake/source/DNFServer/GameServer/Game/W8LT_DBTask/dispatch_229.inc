bool DB_ReportMannerlessUser_Upgrade::dispatch(int userIdx, int slot,
                                               Stream* stream)
{
    // ORIG 0x084276bc `movl $0x0,-0x20(%ebp)` 先清零再赋值（P198/P314 两段形）。
    Packet_Mannerless_User_Accusation* sig = 0;
    sig = stream->GetOutBuffer<Packet_Mannerless_User_Accusation>();
    if (sig == 0)
        return 0;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    // [b28d-solo] P185/P171：ORIG 0x08427701 `lea -0x24(%ebp),%eax; call time`
    // （结果未被 sprintf 使用，死调用照抄）；sql 缓冲 = 0x800 字节
    // （0x0842770c `lea -0xa5e` + `mov $0x800,%ebx` 清零计数实证，非 lea 偏移）。
    time_t occTime;
    time(&occTime);
    (void)occTime;
    // P312/P202①：`= {'\0'}` 复现 4.1 对齐剥离慢路（ORIG 0x0842770c 起）。
    char sql[0x800] = { '\0' };
    char escapedTargetName[0x3b] = { '\0' };
    db->escape_string(escapedTargetName, sig->m_targetName);

    if (sig->isNeedTypeForChattingMsg(sig->m_serverGroup))
    {
        // ORIG 0x084277e6 `mov $0x1ff,%ebx`（-0x25e 起）⇒ 0x1ff 字节。
        char escapedChatMsg[0x1ff] = { '\0' };
        db->escape_string(escapedChatMsg, sig->m_chatMsg);
        sprintf(sql,
                "values(%d, %s, %d, '%s', %d, '%s', now(), '%s')",
                sig->m_serverGroup, NumberToString(sig->m_accIdHi, 0),
                (unsigned int)sig->m_reporterGroup, sig->m_name, (int)sig->m_accIdLo,
                escapedTargetName, escapedChatMsg);
    }
    else
    {
        sprintf(sql,
                "values(%d, %s, %d, '%s', %d, '%s', now(), 'NULL')",
                sig->m_serverGroup, NumberToString(sig->m_accIdHi, 0),
                (unsigned int)sig->m_reporterGroup, sig->m_name, (int)sig->m_accIdLo,
                escapedTargetName);
    }

    db->set_query(
        "inSert into charac_accusation(accusation_type, m_id, server_id, charac_name, server_id_accu, charac_name_accu, occ_time, chatting_msg) %s",
        sql);
    // ORIG 0x08427952 `xor $1,%eax; test %al,%al`（P101：bool==1 折叠）⇒ 早返形。
    if (!db->exec(true))
        return 0;
    return 1;
