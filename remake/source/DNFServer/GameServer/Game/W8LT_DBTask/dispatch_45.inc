bool DB_ReqQueryCharacInfo::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    SIG_QUERY_CHARAC_INFO* sig = stream->GetOutBuffer<SIG_QUERY_CHARAC_INFO>();

    char name[0x3c] = { '\0' };
    db->escape_string(name, (const char*)sig);
    db->set_query(
        "seLect delete_flag,m_id,charac_no,job,lev,grow_type,sex from charac_info where charac_name='%s'",
        name);
    if (!db->exec(true))
    {
        _sendResult(2, slot, 0, 0, 0, 0, 0, 0, 0);
        return 1;
    }

    if (db->get_n_rows() == 0)
    {
        _sendResult(0x15, slot, 0, 0, 0, 0, 0, 0, 0);
        return 1;
    }
    if (db->get_n_rows() == 0)
    {
        _sendResult(0x15, slot, 0, 0, 0, 0, 0, 0, 0);
        return 1;
    }

    int deleteFlag = 0;
    if (!db->fetch() || !db->get_int(0, deleteFlag))
    {
        _sendResult(2, slot, 0, 0, 0, 0, 0, 0, 0);
        return 1;
    }
    if (deleteFlag != 0)
    {
        _sendResult(0x15, slot, 0, 0, 0, 0, 0, 0, 0);
        return 1;
    }

    unsigned int mId = 0;
    int characNo = 0;
    int job = 0;
    int lev = 0;
    int growType = 0;
    int sex = 0;
    db->get_int(1, mId);
    db->get_int(2, characNo);
    db->get_int(3, job);
    db->get_int(4, lev);
    db->get_int(5, growType);
    db->get_int(6, sex);

    // ORIG 0x08426c0e(+0x37a) 实证：local_58[0x14] 缓冲区 memset+memcpy
    // "member_login" 后从未读取（写后死存储，-0x54(%ebp) 仅作 memset/memcpy
    // 目标）。为逐字节还原 ORIG 帧布局（sub $0xdc）与这两次调用保留之。
    char memberLogin[0x14];
    memset(memberLogin, 0, 0x14);
    memcpy(memberLogin, "member_login", 0xd);

    int giftCnt = _getGiftRecvCount(mId);
    _sendResult(0, slot, (const char*)sig, characNo, job, lev, growType, sex,
                giftCnt);
    return 1;
