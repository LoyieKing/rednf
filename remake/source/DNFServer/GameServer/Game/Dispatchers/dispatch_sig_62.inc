// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08220856 orig/ours=134/110 ndiff-blocks=16
//   帧差 sub $220 vs $238 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %ebx | sub    $0x220,%esp
// [SweepDispatchers 2026-09-09] 形态收敛（110→137，ORIG 134；多重集差仅 1 movswl）：
//   ① monsterIdx[256] 改 `= {0}` 复现 ORIG 0x082208d1 内联慢路（旧码 memset call，缺 24 条）；
//   ② 循环条件改 `(int)monsterCount > i`：ORIG 0x082209b6 `movzbl; movzbl %al; cmp i; setg`。
int Dispatcher_DieBloodMonster::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return LineFunc(0xda4b, __PRETTY_FUNCTION__, 0, 0);
    }
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0xda4c, __PRETTY_FUNCTION__, 0, 0);
    }
    unsigned char monsterCount = 0;
    // ORIG 0x082208d1：monsterIdx 0x200 字节为 4.1 内联对齐剥离慢路（非 memset call）。
    unsigned short monsterIdx[256] = {0};   // ORIG -0x212
    if (!packet.get_byte(monsterCount))
    {
        return LineFunc(0xda51, __PRETTY_FUNCTION__, 0, 0);
    }
    // ORIG 0x082209b6：`movzbl count; movzbl %al,%eax; cmp i,%eax; setg`（有符号比较）。
    for (int i = 0; (int)monsterCount > i; i++)
    {
        if (!packet.get_short(monsterIdx[i]))
        {
            return LineFunc(0xda54, __PRETTY_FUNCTION__, 0, 0);
        }
    }
    if (user->get_state() == 5)
    {
        // ORIG 段直证 `call GetPartyIndex; movswl %ax,%ebx`（无栈槽）⇒ P200：
        // 删 partyIdx/gameMgr 具名局部，内联实参直传（原具名形使帧 0x220→0x230）。
        CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
        if (party == 0)
        {
            return LineFunc(0xda5a, __PRETTY_FUNCTION__, 0, 0);
        }
        party->kill_blood_monster(user, (int)monsterCount, monsterIdx);
    }
    return 0;
}