// ============================================================================
// 5. Dispatcher_CallGuildLevelUp::send_packet_guild_levelup  ORIG 0x08200126
//    同 4 型：header(1,0x4c)
// [b28d-solo-DispLinkA1] verdict=IDENTICAL（58=58，纯重定位标签差）。
// ============================================================================
// [P8S3 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
void Dispatcher_CallGuildLevelUp::send_packet_guild_levelup(
    CUser* user, unsigned char errCode)
{
    PacketGuard packet;
    packet.put_header(1, 0x4c);
    if (errCode == 0) {
        packet.put_byte(1);
    }
    else {
        packet.put_byte(0);
    }
    // [SemB02 链接口径残差修正·真语义差异] ORIG 0x08200173 的 `jmp` 落点 = +0x62
    // （`movzbl -0x1c(%ebp),%eax` 即 put_byte(errCode) 三连），即 errCode 的补发在
    // if/else **之外无条件执行**；原实现放在 else 内 ⇒ errCode==0 时我方少发一个
    // 字节（既不等于 ORIG 线格式，也与我方 je 落点 +0x75 = finalize 吻合）。
    packet.put_byte((int)errCode);
    packet.finalize(true);
    user->Send(packet);
}