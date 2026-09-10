int TimerCheckConn::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x8632bca：getUser(p1, 2)（increId 形参被 getUser 忽略，常量 2 为占位）。
    CUser* user = G_CGameManager()->getUser(p1, 2);
    // ORIG 0x8632be4 `cmpl $0x0,-0x24; jne` + 0x8632c05 `setne %al; test; je`
    // ⇒ 两条独立守卫（P133）；旧码 `||` 合并产 mov$1/mov$0 物化岛（多 3 条）。
    // ORIG 0x8632bff：movzwl %ax 后与 p2 作 int 比较（unique_id 低 16 位）。
    if (user == 0)
    {
        return 0;
    }
    if (static_cast<unsigned short>(user->get_unique_id()) != p2)
    {
        return 0;
    }
    std::string accName(user->get_acc_name());
    char ip[0x10] = {0};
    user->m_network.GetPeerIP2(ip, 0x10);
    // ORIG 0x8632cc1 `mov %al,-0x1d(%ebp); cmpb $0x0,-0x1d(%ebp); je`
    // ⇒ 原码有具名 bool 局部（旧码直判 test %al 少 2 条）。
    bool noGameGuard = G_CGameManager()->IsNoGameGuardApplingUser(accName, ip);
    if (noGameGuard)
    {
        // P227 全表达式临时链：ORIG 08632cca 先求 c_str 结果驻 %ebx，再 ctor，最后压
        // 变参（两语句形把求值推到 ctor 之后 ⇒ 少 mov %eax,%ebx）。
        cMyTrace(__PRETTY_FUNCTION__, 0x3a4, 0)(
            "Try To Login By No Game Guard : %s", accName.c_str());
        return 1;
    }
    if (user->get_state() != 0)
    {
        if (user->get_connect_count() <= 2)
        {
            if (user->get_connect_count() > 1)
            {
                // P227：ORIG 08632d72 movswl→%esi（count）、08632d80 getCurCharacName→%ebx
                // 均在 ctor 之前求值保活。
                cMyTrace(__PRETTY_FUNCTION__, 0x3bc, 0)(
                    "User %s - no response %dth ping.", user->getCurCharacName(),
                    static_cast<int>(user->get_connect_count()));
                // P0：ORIG 08632ddc `mov %eax,-0x1c(%ebp); mov -0x1c(%ebp),%eax` 槽往返
                // ⇒ 原码有具名局部（旧码内联实参少 2 条）。
                int resumeTime = GlobalData::s_systemTime_.getCurSec() +
                                 user->GetCheckConnInterval();
                user->SetResumeChecksumTime(resumeTime);
            }
            user->incre_check_count();
            // ORIG 无 SendCheckConnPacket 符号：内联构造空包头（put_header(0,0)+finalize）发送。
            PacketGuard packet;
            packet.put_header(0, 0);
            packet.finalize(true);
            user->Send(packet);
            G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, user->GetUID(),
                                        (TIMER_MESSAGE)0, user->GetCheckConnInterval(),
                                        user->get_unique_id(), 0);
        }
        else
        {
            // ORIG 0x8632ec4：连续 3 次无响应 → 日志 + 断线。
            LogManager::logFormat(1, __PRETTY_FUNCTION__, "TimerDispatcher.cpp", 0x40a,
                                  "User %s - no response during 3 times ping",
                                  user->getCurCharacName());
            user->DisConnSig(static_cast<DISCONN_SIG>(8), false, 0);
        }
    }
    return 1;
