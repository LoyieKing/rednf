// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0821ea32 orig/ours=83/88 ndiff-blocks=13
//   帧差 sub $20 vs $30 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x20,%esp | movl   $&_ZZN30Dispatcher_PowerWarProcessInfo12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | mov    %eax,%ebx
int Dispatcher_PowerWarProcessInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x821ea32 (83 insn)
    if (!(user->get_state() > 2 && user->getCurCharacR() != 0))
    {
        return LineFunc(0xd721,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG 0x0821ea32 段直证（objdump 顺序）：
    //   `call get_charac_no(-1); mov %eax,%ebx` → `call GetServerGroup; call GetServerProxy`
    //   → `mov %ebx,0x4(%esp); mov %eax,(%esp); call SendPowerWarProcessInfo`
    //   ⇒ ① ORIG **无 proxy 判空**（`if (proxy != 0)` 是我方发明防御，§1①）；
    //      ② characNo 先求值（存 %ebx）、proxy 链后求值（P52 实参先物化）。
    // [L7DispG1 2026-09-11] ORIG 0x0821ea4x 段（objdump 序）：`call get_charac_no(-1); mov %eax,%ebx`
    //   → GetServerGroup → GetServerProxy → `mov %ebx,0x4(%esp); mov %eax,(%esp); call SendPowerWar…`
    //   ⇒ 无具名 proxy 槽（旧形落 -0xc 并多 6 条中间指令）；characNo 先求值保活 %ebx、
    //   receiver 链后求值 ⇒ 单条链式表达式。（实测：本件 DIFF 83/84 → AE 83/83；
    //   经 pristine 对照确认**未**牵动下游函数，见 report.md。）
    GlobalData::s_guild_proxy_mgr
        ->GetServerProxy(static_cast<ENUM_SERVER_GROUP>(user->GetServerGroup()))
        ->SendPowerWarProcessInfo(
            static_cast<unsigned int>(user->get_charac_no(-1)));
    PacketGuard response;
    response.put_header(1, 0xec);
    response.put_byte(1);
    response.finalize(true);
    user->Send(response);
    return 0;
}