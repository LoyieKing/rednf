// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08260094 orig/ours=128/132 ndiff-blocks=24
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $0x0,-0x10(%ebp) | lea    -0x10(%ebp),%eax | movl   $&_ZZN39Dispatcher_HeroMissionEventState_Reward12dispatch_sigEP5CUserR9PacketBufE19__PRETTY
int Dispatcher_HeroMissionEventState_Reward::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int missionState = 0;
    if (!packet.get_short(*(short*)&missionState))
    {
        return LineFunc(0xb69,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int missionNo = 0;
    if (!packet.get_short(*(short*)&missionNo))
    {
        return LineFunc(0xb6d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (missionState == 1 && missionNo < 0 && missionNo > 6)
    {
        return LineFunc(0xb71,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // [G1Csig · P0 槽律] ORIG 直证：两分支的 Singleton::Get 结果都只经 `mov %eax,%ebx`
    // 寄存器直传（无 event 具名槽）；我方原写具名 HeroMissionEvent* 使 missionState/
    // missionNo 槽整体下移 8（-0x10/-0x14 → -0x18/-0x1c）⇒ 去具名改链式（同 146 手法）。
    if (missionState == 0)
    {
        ARAD::Singleton<HeroMissionEvent>::Get()->sendMissionData(user);
    }
    else
    {
        char rewarded = (char)ARAD::Singleton<HeroMissionEvent>::Get()->rewardMission(
            user, (HeroMissionCondition::MissionNo::T)missionNo);
        PacketGuard response;
        response.put_header(1, 0x25a);
        // [G1Csig] ORIG 直证：`je <T>` 后 fall-through 为 `movl $0x1,0x4(%esp)`（put_byte(1) 块）
        // —— ORIG 把「rewarded != 0」块排前、`je` 跳「== 0」块；我方原写 `== 0` 正向使两块
        // 顺序相反（6 条差异）⇒ 改用 `!= 0` 正向（同一语义，布局对齐）。
        if (rewarded != 0)
        {
            response.put_byte(1);
            response.put_short(missionState);
        }
        else
        {
            response.put_byte(0);
            response.put_byte(0);
            response.put_short(missionState);
        }
        response.finalize(true);
        user->Send(response);
    }
    return 0;
}