void _NS_PI_2ND_ReconstructCheckSum(unsigned int& checksum, unsigned int k1,
                                    unsigned int k2, unsigned int k3)
{
    // [b28c-lane24] ORIG 0x0808ca2c 无帧（sub 无）、累积在 %edx（`movzbl %al,%edx`
    // 起，逐 `or %eax,%edx`），末 `mov %edx,(%eax)` ⇒ 源码是单一赋值表达式
    //（P0：无具名局部槽）。
    // [b28d-L1] 修法：`(unsigned char)checksum` cast 形使 4.4 产
    // `movzbl %al,%eax; mov k1,%edx; …; or %eax,%edx`（首项目标 %eax，DIFF 4 行）；
    // 改 `checksum & 0xff` 掩码形后 4.4 直接 `movzbl %al,%edx` 与 ORIG
    // 0x0808ca35 逐条一致（探针 /tmp/b28d_l1/probe/p_rc.cpp r1 vs r4：cast 首项
    // 落 %eax、掩码形首项落 %edx）。语义同为取低 8 位。
    checksum = (checksum & 0xff) |
               ((k1 << 8) & 0xffff00) |
               ((k2 << 24) & 0xf000000) |
               (k3 << 28);
