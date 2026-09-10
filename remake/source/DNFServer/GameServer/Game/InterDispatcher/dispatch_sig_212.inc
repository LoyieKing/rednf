int Inter_GuildBoardData::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GuildBoardDataPktView* pPkt = reinterpret_cast<Inter_GuildBoardDataPktView*>(data);
    // ORIG 0x84dee8e/0x84deebc/0x84df274 三处 `mov $0x0,%ebx; jmp <epilogue>`（统一出口
    // `mov %ebx,%eax`）⇒ 原码为三个独立 `return 0`（P16 early-return 形，非单一出口）。
    if (user->get_state() <= 2)                        // cmp $0x2,setle
        return 0;
    if (user->get_charac_no(-1) !=
        pPkt->m_13)
        return 0;

    PacketGuard packet;                                // -0x44

    // §P0/P251：ORIG 对 msg+0xc 无槽往返（比较与 cMyTrace 实参各自重读 movzwl 双段）⇒ 不缓存 code。
    // ORIG 0x84deed4 为 `movzwl; movzwl %ax,%eax; cmp $0x232a,%eax`，4.4 恒折 `cmp $0x232a,%ax`
    // （显式 (unsigned int) 提升实测无效，已回退，P162/N7 封顶）。
    if (pPkt->m_c == 0x232a)
        packet.put_header(1, 0x155);
    else if (pPkt->m_c == 0x232e)
        packet.put_header(1, 0x156);

    if (pPkt->m_a == 0)
    {   // 正常数据分支
        packet.put_byte(1);
        packet.put_byte((int)pPkt->m_e);
        packet.put_byte((int)pPkt->m_17);
        // ORIG 全程以 -0x28 具名槽（pInfo）作基址（084df088 `add -0x28(%ebp),%edx` 等 9 处）
        // ⇒ 循环体基址走 pPkt 槽，不用形参 data。
