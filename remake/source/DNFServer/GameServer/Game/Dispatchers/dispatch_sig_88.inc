// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x082033e6 orig/ours=77/76 ndiff-blocks=11
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: sub    $0x24,%esp | movb   $0x0,-0x15(%ebp) | movw   $0x0,-0x18(%ebp)
int Dispatcher_PeerConnectResult::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x082033e6：连读 get_byte(result)/get_short(uniqueId)，任一失败
    // → LineFunc(0x9a1c / 0x9a1f, 0, 0)；find_from_world(uniqueId)==0 返 0；
    // 取双方 nat_type（movsbl 扩展）+ result 字节 → 统计器 IncreaseNatType 后返 0。
    unsigned char result = 0;
    unsigned short uniqueId = 0;
    if (!packet.get_byte(result))
    {
        return LineFunc(0x9a1c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<short*>(&uniqueId)))
    {
        return LineFunc(0x9a1f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    CUser* peer = G_GameWorld()->find_from_world(uniqueId);
    // [G6-88] P295：ORIG 0x08203xxx `sete %al; test %al,%al; je`（bool 归一岛，非直跳）
    //   ⇒ 用 while 形（K1 已验证：`while(cond){return ...}` 逼 4.4 产 setcc 物化）。
    while (peer == 0)
    {
        return 0;
    }
    // asm：movsbl nat 值 → int（有符号字节扩展）
    int localNat = static_cast<int>(static_cast<signed char>(user->m_network.get_nat_type()));
    int peerNat = static_cast<int>(static_cast<signed char>(peer->m_network.get_nat_type()));
    // ORIG 0x0820346a：GetStatisticsNatType() 结果经 %eax 直传 IncreaseNatType，
    //   无 stats 具名指针槽（我方具名使帧 0x24→0x34、槽整体下移 4）。
    G_GameWorld()->GetStatisticsNatType()->IncreaseNatType(
        localNat, peerNat, (unsigned char)result);
    return 0;
}