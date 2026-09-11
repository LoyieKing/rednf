// [round3-L29 2026-09-10 复核] 算法类（§6 豁免口径）。ORIG 0x080da1d6 为**全栈槽**形
//   （hashA@-0x14/i@-0x8/p@-0x10 槽往返）；加 `register` 会反向变寄存器形（54 条 vs 原 50 条、
//   ndiff 12→58）⇒ 已回退保持槽形。残差 = 两端槽/寄存器分配差（N82 族）。
unsigned long CMTRand::hash(long a, long b)
{
    static unsigned long differ = 0;

    unsigned long hashA = 0;
    const unsigned char* p = (const unsigned char*)&a;
    for (unsigned int i = 0; i <= 3; ++i)
    {
        // [b28d-L14 实证] ORIG 0x080da1d6 分两句（先 `(hashA<<8)+hashA` 落槽，
        // 再 `add p[i],hashA`），循环变量 unsigned（setbe）。
        // [L6 #61-75 CMTRand 2026-09-11] ORIG 0x080da1f2-0x080da1fc 为「先算后存」
        //   （`mov -0x14(%ebp),%edx; mov %edx,%eax; shl $0x8,%eax; add %edx,%eax;
        //     mov %eax,-0x14(%ebp)`）；实测 `hashA = (hashA<<8)+hashA;` 与
        //   `hashA = hashA + (hashA<<8);` 在 4.4 下均折叠为单条 `add %eax,-0x14(%ebp)`
        //   （内存目标 +=，少 2 条）⇒ 4.1 表达式折叠族，4.4 不可复现（N82）。保留原形。
        hashA = hashA + (hashA << 8);
        hashA += p[i];
    }

    unsigned long hashB = 0;
    p = (const unsigned char*)&b;
    for (unsigned int i = 0; i <= 3; ++i)
    {
        hashB = hashB + (hashB << 8);
        hashB += p[i];
    }

    return (differ++ + hashA) ^ hashB;
}