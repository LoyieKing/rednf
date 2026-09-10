void DB_AvatarItemInsert::_SendResult(SIG_AVATAR_INSERT* sig, int slot)
{
    static const int handleIdx = 3;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb18), true);
    guard.operator*()->operator<<((int)0x31);
    guard.operator*()->operator<<((int)slot);
    // ORIG 0x840022x store 区：`call; mov %eax,%edx; mov -0xc,%eax;
    // mov %edx,0x4b(%eax)`（商经 %edx+基址 reload）。cast LHS 形使 4.4 缓存
    // out 于 %ebx 跨 call 并折成单条 `mov %eax,0x4b(%ebx)`；view 型直存变量
    // + (int) 转换实参产 ORIG 三步形（探针 /tmp/w8lt_sr2.o::s4）。
    DB_AvatarInsertView* out =
        reinterpret_cast<DB_AvatarInsertView*>(
            guard.operator->()->GetInBuffer<SIG_AVATAR_INSERT>());
    memcpy(out, sig, 0x53);
    out->m_identity = (int)WongWork::DBCommon::GetIdentity(db);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
