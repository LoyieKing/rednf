// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0821df6a orig/ours=164/168 ndiff-blocks=29
//   帧差 sub $40 vs $50 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x40,%esp | mov    %eax,%ebx | mov    %eax,%esi
int Dispatcher_CheckEnableServerMessage::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821df6a（PacketDispatcher_Impl_1.cpp 0xd236 区）
    if (user->get_state() != 3)
    {
        // [G1Csig · P0 槽律] ORIG 直证：`mov %eax,%ebx`（NumberToString 结果）、
        // `mov %eax,%esi`（get_state 结果）—— 两者均**寄存器保活、无栈槽**；我方原写
        // idStr/state 具名占 2 槽（帧 0x40→0x50，余槽整体下移）⇒ 去具名改内联实参；
        // 且 ORIG ctor 在实参求值之后 ⇒ 全表达式临时链（P227），非两段式具名 trace。
        cMyTrace(__PRETTY_FUNCTION__, 0xd236, 0)(
            "ServerMessage : Dispatcher_CheckEnableServerMessage => User State : %d, m_id : %s",
            user->get_state(), NumberToString(user->get_acc_id(), 0));
        return LineFunc(0xd237,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned char msgIdx = 0;
    if (!packet.get_byte(msgIdx))
    {
        return LineFunc(0xd23d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int curSec = GlobalData::s_systemTime_.getCurSec();
    PacketGuard guard;
    guard.put_header(1, 0xde);
    CSharedServerMessageManager* mgr =
        G_CGameManager()->GetSharedServerMessageManager();
    if (mgr != 0)
    {
        int cond = 0;
        if (mgr->CheckUserCondition(user, (int)msgIdx, cond))
        {
            // [P4 负结论 2026-09-11 实测回滚] 去 (int) cast 的扰动**无效**：复编后
            //   blocks 仍 5、164/164 不变（判据要求 5→3）⇒ 4.4 的实参载入序不受该 cast
            //   控制。恢复显式 (int)（与 get_byte 的 unsigned char 提升逐位等价）。
            if (mgr->IsChangableMessage((int)msgIdx, curSec))
            {
                guard.put_byte(1);
            }
            else
            {
                guard.put_byte(0);
                guard.put_byte(1);
            }
        }
        else
        {
            guard.put_byte(0);
            if (cond != 0)
            {
                guard.put_byte(cond);
            }
            else
            {
                guard.put_byte(3);
            }
        }
    }

    guard.put_byte((int)msgIdx);
    guard.finalize(true);
    user->Send(guard);
    return 0;
}