bool Arad_Dispatcher_Manager::inter_dispatch(int code, CUser* user,
                                             char* body, int len)
{
    // ORIG 0x0818ff6c：查 inter 组，虚槽3 调用；err!=0 时记日志并对 user 断连。
    Arad_Dispatcher* d = m_interDispatchers[code];
    if (d == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Dispatch_Manager.cpp",
            "bool ARAD::Arad_Dispatcher_Manager::inter_dispatch("
            "ARAD::Arad_Dispatcher_Manager::INTERNAL_PACKET_CODE, CUser*, char*, int)",
            0x7d, "Not found Inter_Dispatch_Code[%d]", code);
        return false;
    }
    // ORIG 0x0818ffeb：`cmpl $0,-0x10; je end(mov$1)`＝ err!=0 才进错误块
    // （尾 mov$1 落函数尾，反向 choose 岛不可用 P63/P191）。
    const int err = d->inter_dispatch(user, body, len);
    if (err != 0)
    {
        unsigned int accId = 0;
        if (user != 0)
        {
            accId = user->get_acc_id();
            user->DisConnSig((DISCONN_SIG)2, 1, err + 1000000);
        }
        LogManager::logFormat(
            1, "localjapan/Arad_Dispatch_Manager.cpp",
            "bool ARAD::Arad_Dispatcher_Manager::inter_dispatch("
            "ARAD::Arad_Dispatcher_Manager::INTERNAL_PACKET_CODE, CUser*, char*, int)",
            0x8d, "Inter Dispatch Result Error Inter_Dispatch_Code[%d] user_acc_id[%d]",
            code, accId);
        return false;
    }
    return true;
