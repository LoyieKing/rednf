// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x082226c0 orig/ours=101/99 ndiff-blocks=7
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN30Dispatcher_FightVillageMonster12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movw   $0x0,-0x12(%ebp) | lea    -0x1
int Dispatcher_FightVillageMonster::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // [b28d-solo-Dispatchers] P0：ORIG 无 world 槽（`call G_GameWorld; mov %eax,(%esp);
    //   call IsIntegratedPvPBaseChannel` 直传）⇒ 删具名局部改链式内联。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel()) {
        return 0;
    }
    // 【batch-17 S池 P48/P183】ORIG 两个比较臂各调一次 G_CEnvironment
    // （0x08203xx `call G_CEnvironment` ×2 + `cmp $1/$2`），缓存 serverType
    // 会少 1 call（实测 -5~7 insn）。
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||
        G_CEnvironment()->m_serverEnvir.m_serverType == 2) {
        return 0;
    }
    if (user == 0) {
        return LineFunc(0xdc18, __PRETTY_FUNCTION__, 0, 0);
    }
    if (user->get_state() != 3) {
        return 0;
    }
    unsigned short monsterIdx = 0;
    if (!packet.get_short(monsterIdx)) {
        return LineFunc(0xdc26, __PRETTY_FUNCTION__, 0, 0);
    }
    CParty* party = (CParty*)user->GetParty();
    if (party == 0) {
        party = G_CGameManager()->GetParty();
        if (party != 0) {
            party->set_single_play(user);
        }
    }
    if (party != 0) {
        // [b28d-solo-Dispatchers] P198①：ORIG 在进入块后先 `movl $0x0,-0xc(%ebp)`
        //   （ret 预清零）再 call ⇒ 声明=0 + 独立赋值两句式。
        int ret = 0;
        ret = party->OnFightVillageMonster(user, (int)monsterIdx);
        // [b28d-solo-Dispatchers] P225：ORIG 0x08203xxx `cmpl $0x0,-0xc(%ebp);
        //   setne %al; test %al,%al; je`（4.1 setne 物化）⇒ while+break 形。
        while (ret != 0) {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10b, (unsigned char)(ret & 0xff));
            break;
        }
    }
    return 0;
}