// ORIG 0x081d4886（报告 Dispatcher_RefreshGuildInfo/process.md）：
// state>2 且 guildkey!=0；characNo 经 get_charac_no(-1)。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x081d4886 orig/ours=49/45 ndiff-blocks=9
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: push   %esi | push   %ebx | sub    $0x10,%esp
int Dispatcher_RefreshGuildInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    (void)param;
    // [round5 A01] ORIG 0x081d4899 `setle`(=state<=2 则 return 0) + 0x081d48b7 `sete`
    //   （guildkey==0 则 return 0）：两条负极性独立 guard。
    if (user->get_state() <= 2)
    {
        return 0;
    }
    if (user->get_charac_guildkey() == 0)
    {
        return 0;
    }
    // [round5 A01] P0/P183：ORIG 0x081d4904..0x081d490f 三结果全在寄存器
    //   （%ebx guildkey / %esi characNo / receiver 链最后求值），无具名槽
    //   （旧码三局部 ⇒ 帧 0x10→0x28）。实参右到左：a1=guildkey 先求值。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendRefreshGuildInfo((unsigned int)user->get_charac_no(-1),
                               (unsigned int)user->get_charac_guildkey());
    // [G3 负结论 2026-09-11] 旋钮「两具名 unsigned 局部显式保活」= 劣化：
    //   沙箱读数 49/49 NEAR → 49/47 DIFF（帧与指令序错位，局部落槽）。
    //   本件残余 = N7 寄存器角色互换：ORIG 0x081d48d0 `call guildkey; mov %eax,%ebx`
    //   / 0x081d4904..0x081d490f `%esi characNo`，我方同序但 ebx↔esi 相反
    //   （同 dispatch_sig_159）。源码形已同 ORIG（右到左实参 + 无具名槽）⇒ 封顶。
    return 0;
}