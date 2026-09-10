int BlueMarble::sendFinishBlueMarble() const
{
    int winner = getWinner();
    // b28c-1：P223 尾空标签岛（ORIG 0x080d87f7 `jmp +0xa8; nop` 汇合形）
    //   ⇒ 条件反极性 goto + 内层复合块（避免 goto 跨 packet 初始化声明）。
    if (winner == -1) goto done;
    {
        PacketGuard packet;
        packet.put_header(0, 0x1b1);
        packet.put_byte(winner);
        packet.finalize(true);
        sendToBlueMarble(packet);
    }
done:;
