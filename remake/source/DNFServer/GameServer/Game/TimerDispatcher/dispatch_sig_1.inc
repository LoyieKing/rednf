// [b28d-30] 复核 DIFF：callee-saved/帧继承跨版本差（N7/N82/P210 族），ORIG 0x0863xxx 见 report。
// [b28d-solo-TimerDispatcher] 判词 N82 封顶：全帧槽平移（std::string 槽 -0x4c↔-0x24、
// user -0x24↔-0x1c）+ logFormat varargs 装载差；助记符族同。
// [TimerDispatcher-2 2026-09-09 复核] gap=1 = 1 条 ORIG-only jmp（EH 尾块相对序 N104）；
// 另 esi↔ebx 角色互换（ORIG GetUID→esi / GetCheckConnInterval→ebx vs 4.4 反向）
// ⇒ N7（N109 角色互换）⇒ 封顶。
// [TimerDispatcher-3 2026-09-10 判据①] 14 条 jcc 目标逐条对应；位移差全部来自 ORIG
// `jmp <跳过 catch 的共享 D1Ev 块>`（N104 EH 尾块相对序）与 ss 析构块位置，语义等价。
bool TimerCheckConn::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
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
                    // [LeaTimer28Run 2026-09-11 · §6.5 串盲区] ORIG .rodata 实字节无句点
                    // （`User %s - no response %dth ping\0`，objdump -s 实证）；
                    // 旧源码多一个 '.' ⇒ 运行期日志文本与 ORIG 不同。
                    "User %s - no response %dth ping", user->getCurCharacName(),
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
}