bool DB_Check3rdPartyConcent::dispatch(int userIdx, int slot, Stream* stream)
{
    unsigned int mId = 0;
    *stream >> mId;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    db->set_query("seLect 3dparty_consent from member_join_info where m_id = %s",
                  NumberToString(mId, 0));
    if (db->exec(true) != 1)
        return 0;
    // ORIG 0x08445…：`mov %eax,-0xc`（get_n_rows 落槽）+ `movb $0,-0x15`
    // （consent 在其后声明占 -0x15）+ `cmpl $0,-0xc;jle`——具名槽形
    // （4.4 免槽直判会把整帧内推 4B 且 setg 折叠）。
    int nRows = db->get_n_rows();
    unsigned char consent = 0;
    if (nRows > 0)
    {
        if (db->fetch() != 1)
            return 0;
        if (db->get_ubyte(0, consent) != 1)
            return 0;
    }
    else
    {
        // ORIG 0x08445…：else 路径 `cmpl $0,-0xc;jne; movb $0,-0x15`
        // （==0 判定：jne 跳过清零）。
        if (nRows == 0)
            consent = 0;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xad91), true);
    guard.operator*()->operator<<((int)0x2b7);
    // ORIG 0x08445…：`mov 0x10(%ebp),%edx` = slot（成员函数 a2），非 userIdx。
    guard.operator*()->operator<<((int)slot);
    guard.operator*()->operator<<(consent);   // ORIG lsEh（unsigned char 1 字节，勿转 int）
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
