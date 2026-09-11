// ORIG 0x081d4fcc（报告 Dispatcher_CancelJoinGuild/process.md）：
// state==3 且 guildkey==0；param+0xd(unsigned)=申请索引；
// 成功路径末尾 SendCmdOkPacket(0x15e)。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x081d4fcc orig/ours=48/42 ndiff-blocks=6
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: push   %esi | push   %ebx | sub    $0x20,%esp
int Dispatcher_CancelJoinGuild::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;
    // [round5 A01] ORIG 0x081d4fe2/0x081d4ffd 双负极性 guard（setne 物化 + je body）。
    if (user->get_state() != 3)
    {
        return 0;
    }
    if (user->get_charac_guildkey() != 0)
    {
        return 0;
    }
    // [round5 A01] **真 bug**：ORIG 0x081d4ff0 读的是 `0x10(%ebp)`(msg)+0xd，
    //   本 TU read() 也写 msg+0xd；旧码读 param+0xd 属错对象。
    //   P311/P235：具名成员视图（MSG_CANCEL_JOIN_GUILD::m_characNo@+0xd）才出 ORIG 的
    //   单条 `mov 0xd(%eax),%esi`；裸 `*(uint*)(p+0xd)` 恒折 `add $0xd`+`mov (%eax)`。
    MSG_CANCEL_JOIN_GUILD& req = reinterpret_cast<MSG_CANCEL_JOIN_GUILD&>(msg);   // -0xc
    DB_CancelJoinGuild::makeRequest((int)user->GetUID(),
                                    req.m_characNo,
                                    (unsigned int)user->getCurCharacNo());
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x15e);
    return 0;
}