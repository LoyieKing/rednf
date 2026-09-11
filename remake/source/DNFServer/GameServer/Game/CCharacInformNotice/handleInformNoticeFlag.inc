// ORIG T 0x080e4806：flag&7=bit，flag>>3=byte；check 真则置位并 alter。
// [b28d-solo-CCharacInformNotice] 判词 DIFF(55/54) N 封顶：P223 多岛形——ORIG 三失败
//   出口各落独立 nop 岛（+0x7d nop;jmp / +0x80 nop;jmp / +0x83 nop，标签序=守卫序），
//   负条件 goto 到连续尾标签 L1/L2/L3 逐岛复现。byteIdx 含初始化须包内层复合块
//   （P223 扩展：4.4 拒 goto 跨同层非平凡初始化声明）。
//   残差 N：①第三守卫 ORIG `call; test%al,%al; je` vs 4.4 `call; xor$1; test%al,%al;
//   jne`（P168a①/P292 bool 归一恒物化；`==0`/`==false`/`!f()`/`return` 同形；正向
//   if 可消 xor 但 4.4 前置 if-false 落点并 jump-thread 岛2/岛3 → 51<55 更劣）；
//   ②两处 N18a 反向折叠：ORIG `movzbl 0x5(%ecx,%edx,1),%edx; mov %edx,%ebx` 与
//   `lea (%ebx,%edx,1),%edx; mov %edx,%ecx`，4.4 恒单段直落。
//   P248 三档试尽（探针 /tmp/cn_probe*.cpp）：m_flags 元素 1 字节 enum 化翻正第一处
//   中转（v10/v13/v15），第二处中转与 xor 在 1u/1/+=/具名/register/uchar 截断/enum
//   RHS 共 12 形下均不可翻；4.4.6 后端复测同形（非 N7 后端敏感族）。
// [N5B 2026-09-11 复核 · fresh 口径 DIFF 55/55 gap 0（旧口径 55/54）] 三条残差轴终判：
//   ①`xor $1` 不可消：`!f()`、`f()==false`、`f()!=true`、具名 bool、正向 `f()` 五形探针
//     （p7.cpp）——前四形 4.4 恒发 `xor $0x1,%eax; test %al,%al`，仅正向 if 无 xor 但
//     令岛 2/岛 3 被 jump-thread（51<55 更劣）；
//   ②m_flags 元素改 1 字节 `enum : unsigned char` 确能补回第一处中转（p3.cpp h_enum
//     `movzbl…,%edx; mov %edx,%ebx` ↔ h_uchar 单段），但令我方 56/55、gap 0→**−1 反劣**
//     ⇒ 不采用（消费侧 get/chk 的 uchar 版与 enum 版逐条相同，p4.cpp）；
//   ③第二处中转 4.4 恒 `lea …,%ecx` 直落（p3 的 h_enum 亦如此）⇒ N18a 封顶。
void CCharacInformNotice::handleInformNoticeFlag(unsigned int flag)
{
    unsigned int bit = flag & 7;
    if (bit > 7)
        goto L1;
    {
        unsigned int byteIdx = flag >> 3;
        if (byteIdx >= InformNotice::kInformNoticeIndex)
            goto L2;
        // [G18 T15 2026-09-11 · P354 实证（负例，已还原）] 试 P354「空 then + else」形
        // （`if (f()) { } else { goto L3; }`）：**`xor $0x1` 确被消掉**（our 55→53、
        // 差异行 9→4，推翻旧注「xor 不可消」），但同时暴露 N18a 两处中转缺口，
        // gap 0→**+2**（our 53 / ORIG 55）⇒ 按「四数不劣 + gap 严格改善」闸门判**净劣**，
        // 已还原原形。⇒ 本件宜与 N18a 两处中转（`movzbl…,%edx; mov %edx,%ebx` 与
        // `lea…,%edx; mov %edx,%ecx`）合并攻克后再套 P354。
        if (!checkInformNoticeFlag(bit, byteIdx))
            goto L3;
        m_flags[byteIdx] = (unsigned char)(m_flags[byteIdx] + (1u << bit));
        alter();
        goto END;
    }
L1:;
L2:;
L3:;
END:;
}