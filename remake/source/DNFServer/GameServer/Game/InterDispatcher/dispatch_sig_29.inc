// [b28d-lane2] verdict=DIFF ndiff=221（ORIG 0x084ceaee）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+4。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=228（ORIG 0x84ceaee）：P226 具名 bool/char 落槽+N18a 单段 regmove+N7 间接调用寄存器。
int Inter_IPGResult::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_IPGResultPktView* pPkt = reinterpret_cast<Inter_IPGResultPktView*>(data);
    // P324：ORIG 前段调用序是 PacketGuard ctor → SendCashData → cMyTrace，而 IPG 数据流段
    //   （ProcessIPGUserData / StreamPool::Free）落在函数**后段** ⇒ 原码是
    //   `if (pPkt->m_0 == 0) { 主逻辑 } else { IPG 数据流 }`，不是我方旧稿的提前 return 结构。
    if (pPkt->m_0 == 0)
    {
        char* msg = data + 1;
        CUser* user =
            G_CGameManager()->getUser(pPkt->m_21, 10);
        if (user == 0)
            return 0;

        PacketGuard packet;
        if (pPkt->m_1d == 4)
        {   // 余额变更 → 重发现金数据
            user->SendCashData(true);
        }
        else if (pPkt->m_1d == 6)
        {   // 点数查询结果：经 CIPGHelper 虚槽 +0x2c 回填（类未建模——asm 桥残留保留，按 §8 协议待建模清理）
            typedef void (*QueryFn)(void*, CUser*, int);
            // [K3 2026-09-11] P0：ORIG 084ceb87 段把 s_pIPGHelper 全局地址读两次、query 只在
            //   %ecx 中转，无 helper 槽（旧稿 `void* helper` 多 2 条槽往返）。
            // [R5a01 2026-09-12 · ④ 真语义 bug] 旧码 `*(QueryFn*)((*(void***)g)[0x2c/4])` **多一层
            //   解引用**：ORIG 0x084ceb8c `mov (%eax),%eax` / 0x084ceb8e `add $0x2c,%eax` /
            //   0x084ceb91 `mov (%eax),%ecx`（vtable[11] 本体即函数指针，**2 层**）/
            //   0x084ceba2 `call *%ecx`。旧码 3 层 ⇒ 把函数指针的值当地址再取一次首 4 字节 ⇒
            //   间接调用目标错。改单层 + 内联进调用表达式（同
            //   TimerDispatcher/dispatch_sig_92.inc 已修先例）。
            (*reinterpret_cast<QueryFn*>(
                *reinterpret_cast<char**>(GlobalData::s_pIPGHelper) + 0x2c))(
                    GlobalData::s_pIPGHelper, user, 10);
            // P328⑩/P227：ORIG 0x84ceba4..0x84cebea 先求 msg+0xb3→槽、m_b2→%edi、
            // msg+0x64→%esi、msg+0x3c→%ebx 再 ctor ⇒ 全表达式临时链。
            cMyTrace(__PRETTY_FUNCTION__, 0x2b65, 0)(
                "[Taiwan, Billing] ChargePointQuery Result:%s, %s, %d, %s",
                msg + 0x3c, msg + 0x64, (int)(char)pPkt->m_b2, msg + 0xb3);
            return 0;
        }
        else if (pPkt->m_1d == 0)
        {   // CERA 查询结果日志
            // ORIG 0x84cec29..0x84cec6f 先求 msg+0x28→%ebx、msg+0x50→%esi、msg+0x26e→槽、
            // m_26e→%edi 再 ctor ⇒ 全表达式临时链。
            cMyTrace(__PRETTY_FUNCTION__, 0x2b6e, 0)(
                "[Taiwan, Billing] ChargeCeraQuery Result:%s, %s, %s, %d",
                msg + 0x28, msg + 0x50, msg + 0x26e,
                (int)pPkt->m_26e);
            return 0;
        }
        else
        {   // 其余状态 → 0x43 错误状态包
            packet.put_header(1, 0x43);
            packet.put_byte(0);
            packet.put_byte(0);
            packet.put_short(0x19);
            packet.put_byte(0);
            packet.put_byte(0x19);
            packet.put_int(0);
            packet.put_int(0);
            packet.put_int(0);
            packet.put_int(0);
            packet.put_int(-1);
            packet.finalize(true);
            user->Send(packet);
        }

        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2b96, "IPG ERROR (%s)", "IPG Network Error");
        return 0;
    }

    // IPG 数据流结果：转 ItemVendingMachine 处理后归还 Stream
    char* reqData = data + 1;
    char* resData = data + 0x2dd;
    CUser* target =
        G_CGameManager()->getUser(pPkt->m_21, 11);
    Stream* stream = *reinterpret_cast<Stream**>(reqData + 0x2d8);
    if (target != 0)
    {
        GlobalData::s_pItemVendingMachine->ProcessIPGUserData(
            target,
            reinterpret_cast<WongWork::IPG::SIPGData*>(resData),
            reinterpret_cast<WongWork::IPG::SIPGData*>(reqData),
            stream);
    }
    GlobalData::s_stream_pool->Free(stream);
    return 0;
}