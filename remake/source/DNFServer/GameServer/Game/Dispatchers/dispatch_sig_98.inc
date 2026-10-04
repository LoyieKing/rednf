// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x082051d0 orig/ours=124/128 ndiff-blocks=6
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN29Dispatcher_CharacterStatistic12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | lea    -0x20(%ebp),%eax | add    $0x8,
int Dispatcher_CharacterStatistic::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x82051d0：get_state < 3 → 0；依次 get_int 6 个字段（任一失败 →
    // LineFunc(0x9d1f+k, __PRETTY_FUNCTION__, 0, 0) 并返回其返回值），
    // 成功后写 CUser+0x8e03c..0x8e050（6 个 int，Character 统计区），并置
    // CUser+0x8e074 = 1。零外部调用（自包含）。
    if (user->get_state() < 3)
    {
        return 0;
    }

    // ORIG 0x082051d0：6 个槽连续（-0x20..-0xc）且读取点用 `lea -0x20(%ebp)[+k*4]`
    //   ⇒ 原码是数组 `int values[6]`；写回用 `mov %edx,0x8e03c(%eax)` 直 disp store
    //   ⇒ 无 base 具名指针（我方旧形 6 独立变量 + base 视图多 4 槽且产 lea 链）。
    // [G6-98] 语义修复（字段错位）：ORIG 0x08205237/0x0820527c/0x082052c1 依次取址
    //   -0x18/-0x14/-0x1c ⇒ 读序 = 声明序 2,3,4（即 f10→f14→f0c 字段名序），
    //   则包中第 2 个 int 落 +0x8e044（旧译落 +0x8e040：包第 2/3/4 个 int 全部错位）。
    int values[6];
    if (packet.get_int(values[0]) != 1) return LineFunc(0x9d1f, __PRETTY_FUNCTION__, 0, 0);
    if (packet.get_int(values[2]) != 1) return LineFunc(0x9d20, __PRETTY_FUNCTION__, 0, 0);
    if (packet.get_int(values[3]) != 1) return LineFunc(0x9d21, __PRETTY_FUNCTION__, 0, 0);
    if (packet.get_int(values[1]) != 1) return LineFunc(0x9d22, __PRETTY_FUNCTION__, 0, 0);
    if (packet.get_int(values[4]) != 1) return LineFunc(0x9d23, __PRETTY_FUNCTION__, 0, 0);
    if (packet.get_int(values[5]) != 1) return LineFunc(0x9d24, __PRETTY_FUNCTION__, 0, 0);

    // [G5DsigE] 裸偏移 → 具名成员（Character = CUser+0x8e034；IData 零大小，故
    //   +0x8/0xc/0x10/0x14/0x18/0x1c = PlayInfo::m_f08/m_f0c/m_f10/m_f14/m_f18/m_f1c，
    //   与 CParty.cpp:3674 的 `user+0x8e054 = m_f20` 直证一致）。
    user->m_character.m_f08 = values[0];
    user->m_character.m_f10 = values[2];
    user->m_character.m_f14 = values[3];
    user->m_character.m_f0c = values[1];
    user->m_character.m_f18 = values[4];
    user->m_character.m_f1c = values[5];
    user->m_hasCharacterStatistic = 1;

    return 0;
}