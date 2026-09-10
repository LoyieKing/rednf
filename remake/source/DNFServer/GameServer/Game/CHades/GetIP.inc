void CHades::GetIP(char* out)
{
    // b28d-solo-CHades verdict=DIFF [N102] ORIG 0x084b9774 orig/ours=40/39，
    // 唯一残差 = ORIG `add $0x8e418,%eax; movzbl (%eax),%eax` 两步数组退化读被
    // 4.4 折成单条 `movzbl 0x8e418(%eax),%eax`（N102：下标/*arr/指针局部/数组
    // 引用/嵌套成员八形探针全折 disp，唯裸偏移出 add 而 §4.3 禁）。
    // 两处 nop 岛（+0x84/+0x87）已由下方双否定 goto + L1/L2 尾空标签串复现
    // （P223/BM8），strncpy 源实参的数组退化 `add` 形亦已同构。
    m_user->m_network.GetPeerIP2(out, 4);
    // [b28c-16] P223/BM8 尾标签串：ORIG `jne +0x84(nop)/je +0x87(nop)/体尾
    // jmp +0x88` 三岛 ⇒ 两否定 goto + L1/L2 空标签串（4.4 对嵌套 if 恒
    // jump-thread 合并出口）。
    if (out[0] != '\0')
        goto L1;
    if (m_user->m_mcap.m_pcRoomIp[0] == 0)
        goto L2;
    memset(out, 0, 0x10);
    strncpy(out, m_user->m_mcap.m_pcRoomIp, 0x10);
L1:
    ;
L2:
    ;
