// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d7fb6 orig/ours=150/151 ndiff-blocks=21
//   帧差 sub $4c vs $50 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | sub    $0x4c,%esp | mov    $0x0,%ebx
int Dispatcher_CharacSlotExtendEffect::process(CUser* user, MSG_BASE& msg, ParamBase& paramBase)
{
    (void)msg;
    stCharacSlotExtendEffect& param = *reinterpret_cast<stCharacSlotExtendEffect*>(&paramBase);
    // [b23S3C] N：调用点级两试均劣化（极性对调+guard 链式版 fresh .o ndiff 145 > HEAD
    // 132，同口径基线 /tmp/tu_DispatchersBase.o）——ORIG trace 段参数在 cMyTrace ctor
    // 前以 edi/esi/ebx 跨 call 缓存、send 段 stream/sig 走纯寄存器直传，4.4 恒落槽
    // （N18a/N76/setcc 布局差），语义已核无缺口。保留 HEAD 形。
    if (user->get_state() != 2)
    {
        param.m_4 = 7;
        return 0;
    }
    // ORIG 0x081d7fb6：state==2 才处理；slotEffectCount < characSlotLimit 时扩槽并入 DB_Q。
    if (user->getSlotEffectCount() < user->getCharacSlotLimit())
    {
        user->setSlotEffectCount(user->getCharacSlotLimit());
        param.m_8 = static_cast<unsigned char>(user->getSlotEffectCount());
        param.m_9 = static_cast<unsigned char>(user->getCharacSlotLimit());
        Stream* stream = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x429a);
        CStreamGuard guard(stream, true);
        (*guard)->operator<<(0x28a);
        (*guard)->operator<<(-1);

        char* buf = reinterpret_cast<char*>(guard->GetInBuffer<SIG_EXTEND_CHARAC_SLOT>());
        *reinterpret_cast<int*>(buf + 0) = static_cast<int>(user->get_acc_id());
        buf[4] = static_cast<char>(user->getSlotEffectCount());
        buf[5] = static_cast<char>(user->getCharacSlotLimit());
        GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
    }
    else
    {
        unsigned char limit = user->getCharacSlotLimit();
        unsigned char count = user->getSlotEffectCount();
        unsigned int accId = user->get_acc_id();
        char* id = NumberToString(accId, 0);
        cMyTrace trace(__PRETTY_FUNCTION__, 0x4291, 0);
        trace("CharacSlotExtendEffect m_id(%s), SlotEffectCount(%u), CharacSlotLimit(%u)",
              id, (unsigned int)(count & 0xff), (unsigned int)(limit & 0xff));
        param.m_4 = 0x12;
    }
    return 0;
}