bool DB_ReqChangeCharacName::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return false;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    SIG_CHANGE_CHARAC_NAME* sig = stream->GetOutBuffer<SIG_CHANGE_CHARAC_NAME>();

    char name_buf[60] = { '\0' };   // 转义缓冲区（ORIG rep stos 清零 60 字节）
    db->escape_string(name_buf, sig->m_name);

    // env+0x68 = m_dbConnInfo：ORIG 直访成员（经 G_CEnvironment()+0x68 做
    // map::operator[] this，无 accessor call），故禁 get_db_conn_info() accessor。
    typedef std::map<ENUM_DB_HANDLE_IDX, STDBConnInfo> DbConnMap;
    ENUM_DB_HANDLE_IDX connIdx = DB_HANDLE_2;   // ORIG 栈上局部初始值 2
    int ret = _hasSameCharacName(
        db,
        G_CEnvironment()->m_dbConnInfo[connIdx].m_db,
        name_buf);
    if (ret != 0)
    {
        _sendResult(sig, slot, ret);
        return true;
    }

    db->set_query("upDate charac_info set charac_name='%s' where charac_no=%d and delete_flag=0",
                  name_buf, sig->m_characNo);
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        _sendResult(sig, slot, 2);
        return true;
    }

    MySQL* guildDb = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8, E_SERVER_GROUP_NONE);
    // ORIG movzbl 0x2c(%eax)（8 位读）；m_arg2c 为 uint 成员，用 P82 尾部视图折叠为单 disp。
    struct ChangeCharacNameByteView
    {
        char m_pad[0x2c];
        unsigned char m_serverId;
    };
    guildDb->set_query("seLect guild_id from guild_member where charac_no=%d and member_flag=1 "
                       "and server_id=%d",
                       sig->m_characNo,
                       reinterpret_cast<ChangeCharacNameByteView*>(sig)->m_serverId);
    sig->m_arg28 = 0;
    if (!(guildDb->exec(true) && guildDb->fetch()))
        sig->m_arg28 = 0;
    if (guildDb->get_n_rows() == 0)
        sig->m_arg28 = 0;
    else if (!guildDb->get_uint(0, sig->m_arg28))
        sig->m_arg28 = 0;
    _sendResult(sig, slot, 0);
    return true;
