// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08224ab4 orig/ours=117/92 ndiff-blocks=10
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    -0x36(%ebp),%eax | mov    $0x1e,%ebx | mov    $0x0,%edx
// [SweepDispatchers 2026-09-09] 同 RemovePvPBuddy：name 清零改 `char name[0x1e]={0}`（ORIG
//   0x08224ad2 内联慢路）并前置于 type/nameLen；92→117 = ORIG 117。
int Dispatcher_ResponseAddPvPBuddy::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (!G_GameWorld()->IsIntegratedPvPBaseChannel())
        return 0;
    // ORIG 0x08224ad2：name 清零（-0x36，4.1 内联慢路）先于 type/nameLen 初始化。
    char name[0x1e] = {0};                           // -0x36（ORIG 清零 0x1e 字节）
    char type = 0;                                   // -0x37
    int nameLen = 0;                                 // -0x3c
    if (!packet.get_byte(type))
        return LineFunc(0xdf53, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(nameLen))
        return LineFunc(0xdf56, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(name, 0x1e, nameLen))
        return LineFunc(0xdf59, __PRETTY_FUNCTION__, 0, 0);
    GlobalData::s_community_proxy->SendResAddPvPBuddy(
        user->get_acc_id(),
        (unsigned int)user->getCurCharacNo(),
        type,
        name,
        nameLen);
    return 0;
}