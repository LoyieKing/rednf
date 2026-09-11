// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0819f1ec orig/ours=172/177 ndiff-blocks=24
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    -0x19(%ebp),%eax | lea    -0x19(%ebp),%eax | movzbl -0x10(%ebp),%eax
int Dispatcher_AradJumpingCharacter::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x59b;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    // ORIG 0x0819f1e0：SigAradJumpingCharacter sig（C1 0x081a0990 仅清 m_head[0..8]）；
    // 布局 +0x01 acc_id、+0x05 charac_no、+0x09 eventKind、+0x0a growType、
    // +0x0b itemIndex、+0x0d tail(int)；put_binary(&sig, 0x11)。
    arad::SigAradJumpingCharacter sig;
    if (packet.get_byte(reinterpret_cast<unsigned char&>(sig.m_eventKind)) != 1)   // +0x09
    {
        return 0x5a9;
    }
    // ORIG：b1==0 放行（test/je 早跳），仅无符号 >2 报错。
    if ((unsigned char)sig.m_eventKind != 0 && (unsigned char)sig.m_eventKind > 2)
    {
        return 0x5ac;
    }
    if (packet.get_byte(sig.m_growType) != 1)   // +0x0a
    {
        return 0x5af;
    }
    // ORIG：b2==0 放行，仅无符号 >4 报错。
    if (sig.m_growType != 0 && sig.m_growType > 4)
    {
        return 0x5b1;
    }
    if (packet.get_short(sig.m_itemIndex) != 1)   // +0x0b
    {
        return 0x5b4;
    }
    if (packet.get_int(reinterpret_cast<unsigned int&>(sig.m_tail[0])) != 1)   // +0x0d
    {
        return 0x5b6;
    }
    // ORIG 0x0819f2fd：两次 get_state() 调用（未缓存），OR 链 bool 形态。
    if (user->get_state() < 3 || user->get_state() > 4)
    {
        return 0;
    }
    // ORIG 0x0819f44c checkWorkState：失败（返回 false）→ 0。
    if (!checkWorkState(user, sig))
    {
        return 0;
    }
    *(unsigned int*)&sig.m_head[1] = (unsigned int)user->get_acc_id();      // +0x01
    *(unsigned int*)&sig.m_head[5] = (unsigned int)user->get_charac_no(-1); // +0x05
    // [G1Csig · P0 槽律] 同 149/146 判据：ORIG 0x0819f1e0 段无 Stream* 具名槽
    // （我方原 -0xNN 往返 + sig 基址下移 5 字节）⇒ Acquire 结果直传 CStreamGuard。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localjapan/Arad_PacketDispatcher.cpp", 0x5c8), true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x2d, (int)user->GetUID());
    guard.operator->()->put_binary((void*)&sig, 0x11);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), guard);
    return 0;
}