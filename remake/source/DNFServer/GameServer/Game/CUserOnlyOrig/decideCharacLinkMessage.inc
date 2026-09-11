// ---------------------------------------------------------------------------
// 14. CUser::decideCharacLinkMessage() T 0x08652a58
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x08652a58：调用点/形态已对齐（本波修法见函数内注释）。
void CUser::decideCharacLinkMessage()
{
    // ORIG 0x08652a64-0x08652a6b：call 结果 %al 直判 `test; jne END`——无赋值中转
    // ORIG 0x08652a69 `test %al,%al; jne END`：truthy 直判早退（技巧6 变体）
    if (isCharacLinkMessageFlag())
        return;
    {
        // ORIG 0x08652ae2：cond 每轮重调 size() + `cmp -0xc,%eax; setg`（signed
        // int 比较）；0x08652a8d operator[] 结果 %eax 直落 movzwl 免槽
        for (int i = 0; i < (int)m_characList.size(); ++i) {
            // ORIG 0x08652a92：`movzwl 0x27(%eax),%eax; cmp $0x3b,%ax; jle FALSE岛`
            // （显式 (unsigned short) cast，16 位比较免槽）+ 0x08652aa2
            // `xor $0x1,%eax; test %al; je FALSE`（技巧1 !boolcall）+
            // [mov$1][mov$0] 物化岛（技巧40 && 短路）→ register bool asm("al")
            // ORIG 0x08652a96 `cmp $0x3b,%ax; jle`：movzwl 装载后 16 位**有符号**
            // 比较（4.1 load-mode 产物）⇒ 外层 short 化保 signedness。
            // [b28d-solo-CUserOnlyOrig P226/P0] ORIG 短路物化岛后 `test %al,%al; je`
            // 无槽 ⇒ 原码是内联条件（旧具名 doSend 生 -0x9 槽 + cmpb 回读、帧 +4）。
            if (({
                    (static_cast<short>(static_cast<unsigned short>(
                         m_characList[i].m_level)) >= 0x3c)
                    // ORIG 0x08652a4f `xor $0x1,%eax; test %al; je`——显式 ^1 复现
                    && (isCharacLinkMessageFlag() ^ 1);
                })) {
                sendCharacLinkMessage();
                saveCharacLinkMessageFlag(true);
                return;
            }
        }
    }
}