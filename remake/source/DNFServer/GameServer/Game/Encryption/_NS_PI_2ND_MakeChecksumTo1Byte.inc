void _NS_PI_2ND_MakeChecksumTo1Byte(char* p)
{
    // [round5-A26 判词·N18a] ORIG 0x0808cc17 形：p0 先装载入 %edx 并在**末尾**
    // `xor %eax,%edx` 合流，右链 ((p1^p2)^p3)^0x18 落 %eax（4.1 先 expand op0 再 op1，
    // 且常量 xor 在链尾）。本轮三形实测（21/21 条，仅寄存器镜像）：
    // ①`p1^p2^p3^0x18^p0`：链落 %ecx、p0 落 %edx、常量被 4.4 提链尾；
    // ②`p0 ^= (p1^p2^p3^0x18)`（复合赋值形）：23 条，4.4 把 p0 保活入 %ebx（多 push/pop）；
    // ③本形（p0 前置 5 项平铺）：链落 %edx、p0 落 %ecx —— 与①互为镜像，同 21 条。
    // ⇒ 4.4 恒把 5 项链尾项/常量做链尾，与 4.1 的「p0 最外层」树形不可同构，封顶。
    // [LaneB-P345 试形4·已回滚] 显式括号 `p0 ^ (p1^p2^p3^0x18)`（p0 提最外层左操作数）
    //   实测 21→**23** 条 / ndiff 15→17（劣化）⇒ 回滚。判词维持：4.4 对 5 项 xor 链
    //   恒把链尾项/常量做链尾，与 4.1 的「p0 最外层」树形不可同构（N18a）。
    ((unsigned char*)p)[0] =
        (unsigned char)(((unsigned char*)p)[0] ^ ((unsigned char*)p)[1] ^
                        ((unsigned char*)p)[2] ^ ((unsigned char*)p)[3] ^ 0x18U);
}