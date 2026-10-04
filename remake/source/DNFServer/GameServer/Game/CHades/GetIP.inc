// ORIG 0x084b9774（GetIP.md）：peer IP 抓取失败（空串）且 CMCAPManager 内
// PC房 IP(+0x28) 非空时，清零输出缓冲后拷贝该 IP。
void CHades::GetIP(char* out)
{
    // b28d-solo-CHades verdict=DIFF [N102] ORIG 0x084b9774 orig/ours=40/39，
    // [L6S4 round5b 复核] 读数维持 40/39，残差仍唯一（`add $0x8e418` 两步）。
    // 唯一残差 = ORIG `add $0x8e418,%eax; movzbl (%eax),%eax` 两步数组退化读被
    // 4.4 折成单条 `movzbl 0x8e418(%eax),%eax`（N102：下标/*arr/指针局部/数组
    // 引用/嵌套成员八形探针全折 disp，唯裸偏移出 add 而 §4.3 禁）。
    // 两处 nop 岛（+0x84/+0x87）已由下方双否定 goto + L1/L2 尾空标签串复现
    // （P223/BM8），strncpy 源实参的数组退化 `add` 形亦已同构。
    // [M4FreeNext15C.M4S1 round5 复验 · 判 N4/N102 封顶（父泳道定性）]
    // ORIG 0x084b97aa `mov 0x20(%eax),%eax` / 0x084b97ad `add $0x8e418,%eax` /
    // 0x084b97b2 `movzbl (%eax),%eax`（两步 add 展开）；我方
    // `movzbl 0x8e418(%eax),%eax` 单条 disp。本轮新探针同旗标实编：
    //   g1 `arr[0]`、g2 `*arr`、g4 `*&arr[0]` ⇒ 全 fold 成 disp（同我方）；
    //   g3 具名指针局部 ⇒ 出 add 但附 `mov %eax,-0x4(%ebp)` 栈物化（ORIG 无）；
    //   g5 裸 `register` 指针 ⇒ 出 `lea 0x8e400(%eax),%ebx` + `push %ebx`（ORIG 无 push、用 add）。
    // ⇒ 无合法源形可复现「裸 add + 直 deref、无栈无 push」；0x8e418 属
    //   CNetwork<4096,450000> 内部缓冲偏移（非本 TU 布局），P311 数组视图不适用。
    // [P6S3 round6 · 钥匙 §2①②/P341 指针加法和形，落地] 上述 g1..g5 漏了
    // **「基址 + (元素地址 − 基址)」显式指针加法**形：探针 /tmp/p6/sc/p1.cpp
    // D/E 两形在同旗标下逐条产 `mov 0x20(%eax),%eax; addl $0x8e418,%eax;
    // movzbl (%eax),%eax`（零栈物化、无 push，即 ORIG 形；g1 式成员下标恒 fold）。
    // ⇒ 用等值指针表达式替换下标读，语义不变（值仍为 m_pcRoomIp[0]）。
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
}