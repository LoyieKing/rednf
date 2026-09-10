int CCeraShop::LogError(const stErrorLogInput_t& input) const
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("CeraShop.cpp", 0x7a),
                       true);

    (**guard).operator<<(0x32e);          // operator*() → operator<<(int)
    // [b28c-L7] 负例：P226 裸语句表达式形 `({ *(guard.operator*()); })` 实测
    // **反劣**（4.4 额外物化 guard 临时 + 双 landing pad，ndiff 3→+20）。
    // ORIG 0x080c8c46 段 `mov (%eax),%ebx` 单段直落 callee-saved，4.4 恒
    // `mov (%eax),%eax; mov %eax,%ebx` 两段（N18a 反向），源形不可造 ⇒ 封顶。
    (**guard).operator<<(static_cast<unsigned int>(input.m_field0));  // uint 形参

    SIG_CERASHOP_ERROR_LOG sig;           // ctor 0x08328110 清零 +0..+0x18
    sig.m_field0 = input.m_field0;
    sig.m_field4 = input.m_field4;
    sig.m_field14 = input.m_field8;
    sig.m_field10 = input.m_field10;
    // ORIG 0x08320408：call G_CEnvironment → mov 0x378(%eax),%eax（dword 读
    // = m_serverEnvir.m_serverGroup）→ movzbl %al 截断；具名访问复现单 disp 读。
    sig.m_field8 = static_cast<unsigned char>(
        G_CEnvironment()->m_serverEnvir.m_serverGroup);
    sig.m_fieldc = G_CEnvironment()->get_channel_no();
    sig.m_field18 = input.m_fieldc;

    guard.operator->()->put_binary(&sig, 0x1c);   // P103 operator-> 链
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
