int Dispatcher_Mercenary_Return::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MsgMercenaryReturn& req = reinterpret_cast<MsgMercenaryReturn&>(msg);
    SendMercenaryReturnResult& result = static_cast<SendMercenaryReturnResult&>(param);
    if (user == 0)
        return -1;
    if (user->get_state() != 3)
        return -1;

    char* pResult = reinterpret_cast<char*>(&result);
    // ORIG: Return_Mercenary_Charac(user, *(uint*)(msg+0x10), m_result@+0xc 引用传入)
    // （ORIG 0x0868e692 声明 bool、实现 void 转发无 return，%al 残留直写 pResult+0x04）
    struct MercView          // 自然对齐即达 m_field10@+0x10（0xd+1+2=16 已 4 对齐）
    {
        char m_head[0xd];
        char m_field0d;          // +0x0d byte
        char m_pad0e[2];         // +0x0e..+0x0f
        unsigned int m_field10;  // +0x10
    };
    MercView& mv = reinterpret_cast<MercView&>(req);
    pResult[0x04] = static_cast<char>(
        user->Return_Mercenary_Charac(mv.m_field10, result.m_result));
    pResult[0x05] = mv.m_field0d;                               // +0x05 char 回显(msg+0xd)
    *reinterpret_cast<int*>(pResult + 0x08) = mv.m_field10;     // +0x08 int = msg+0x10
    return 0;
