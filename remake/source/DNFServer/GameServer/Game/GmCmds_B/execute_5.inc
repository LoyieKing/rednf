// ORIG execute 0x084a905e：读 AdditionalInfo 移速/攻速/施速，组串回发本人。
// b28d-21: 已修 P214③（255 字节缓冲 `memset` → `char buf[255] = {0}` 复现 ORIG
// 内联「对齐剥离 + rep stos」39 条，ndiff 92→9）；余 N1/槽（ORIG -0x1c add 形 vs
// 重建 disp 折叠）。ORIG 0x084a905e。
void CCharacStatCmd::execute()
{
    // b28d-21：ORIG 0x084a905e 对 255 字节缓冲发内联「对齐剥离+rep stos」
    //（0x84a9063 起 39 条），4.4 对 memset 恒发 call ⇒ 按 P214③ 用大数组
    // 显式零初始化 `= {0}` 复现。
    char buf[255] = {0};                                  // ORIG -0x127(%ebp)

    // b28d-22：N107 族 —— ORIG 三处附加信息字段是单条 disp（mov/movzwl），
    // 原 `unsigned char*` + `add + 0xNN` 退化成 add+mov 两步；改 pack(1) 视图。
    CharacAddInfoStatView const* add = static_cast<CharacAddInfoStatView const*>(
        GetUser()->getCurCharacAddInfoR());
    sprintf(buf, "charac_stat move_speed=%d,attack_speed=%d,cast_speed=%d",
            add->m_moveSpeed,
            (unsigned int)add->m_attackSpeed,
            (unsigned int)add->m_castSpeed);

    PacketGuard packet;
    packet.put_header(0, 0xc);
    packet.put_byte(0);
    packet.put_short(0);
    packet.put_byte(0);
    packet.put_int(strlen(buf));
    packet.put_str(buf, strlen(buf));
    packet.finalize(true);
    GetUser()->Send(packet);
}