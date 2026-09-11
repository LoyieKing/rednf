// ---------------------------------------------------------------------------
// LogError —— ORIG T 0x0832033a (0x13f)
// StreamPool 取流 → guard.operator*() 两次分别 operator<<(0x32e)(int 形参) 与
// operator<<(input[0])(unsigned int 形参，ORIG 0x080e2292) → SIG ctor
// (0x08328110 清零) + 7 字段按 ORIG 写入序（+0/+4/+0x14/+0x10/+8/+0xc/+0x18）→
// guard.operator->()->put_binary(&sig,0x1c)（P103 链保留）→ put(DB_Q) → 返回 1
// （P159 尾部 mov $1 跨 D1 保活）。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_CeraShopDeps] verdict=AE（check_tu_routed DIFF→AE；修法=P248 档③
// 字段同型化：stErrorLogInput_t::m_field0 与 SIG_CERASHOP_ERROR_LOG::m_field0
// int→unsigned int，消 4.4 的 `mov (%eax),%eax; mov %eax,%ebx` 两段，复现 ORIG
// 0x08320394 单段直落 callee-saved。p167 CLEAR）。
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
}