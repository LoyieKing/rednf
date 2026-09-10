int Dispatcher_CheckGuildNameDouble::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // [b28c-34] N104 封顶（EH 尾块相对序，171 vs 170）：ORIG 0x0820879a 正常路径
    // `call CStreamGuardD1; jmp +0x2b2`（无 nop），unwind 块居中；4.4 把同段
    // epilogue 就地内联（少 1 条 jmp）。块内容逐条全等，仅尾块相对序差（P/N104）；
    // 试形：PacketGuard 作用域内移/独立块/goto 尾标签均不翻转。
    // ORIG 0x082084e8 [0x082084e8, 0x082087a3]
    if (!(user->get_state() > 2 && user->getCurCharacR() != 0))
    {
        return LineFunc(0xa369,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int len = 0;                                             // -0x14（size_t 视图）
    if (!packet.get_int(len))
    {
        return LineFunc(0xa37e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (len < 0 || 0x16 < len)
    {
        return LineFunc(0xa381,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char* name = user->getCreateGuildName();                 // -0x10
    if (name == 0)
    {
        return LineFunc(0xa386,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(name, 0x17, len))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9f, 0x6a);
        return 0;
    }
    if (!checkValidCharacName(name))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9f, 0x6c);
        return 0;
    }
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "PacketDispatcher_Impl_1.cpp", 0xa39d), true);
    **guard << 0xeb;
    **guard << static_cast<int>(user->GetUID());
    SIG_DOUBLE_CHECK_GUILD_NAME* sig = guard->GetInBuffer<SIG_DOUBLE_CHECK_GUILD_NAME>();
    memset(sig, 0, 0x17);
    memcpy(sig, name, len);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
