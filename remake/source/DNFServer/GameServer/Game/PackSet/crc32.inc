unsigned int Crc32::crc32(unsigned int crc, char const* buf, unsigned int len)
{
    // b28d-solo-PackSet 判词（AE / ndiff=0，ORIG 0x08ad34ae size 0x55）：①body
    // 的 `crc>>8` 求值序由 `(crc>>8) ^ table[...]` 书写序翻正（P243）；②cond 段
    // `cmp;setne;subl $1,len;test;jne`（旧值先物化、后置递减）由
    // `while (len-- != 0)` 复现——前轮「N82 不可翻」判词作废：for/while/前置三形
    // 确不可翻，**后置递减写进条件**才是唯一解。
    if (crc_table[255] == 0)
    {
        make_crc_table();
    }
    crc = ~crc;
    // b28d-solo-PackSet（P243 同族）：ORIG body 首两条即 `mov crc,%eax; mov %eax,%edx;
    // shr $8,%edx`（右操作数 `crc>>8` 先求值）⇒ 源把 `crc >> 8` 写在 `^` 左侧。
    // b28d-solo-PackSet（P225 变体）：ORIG cond 段为 `cmp; setne %al; subl $1,len;
    // test %al,%al; jne`（旧值先物化、后置递减）⇒ `len--` 写进条件。
    while (len-- != 0)
    {
        crc = (crc >> 8) ^ crc_table[(static_cast<unsigned char>(*buf) ^ crc) & 0xffu];
        ++buf;
    }
    return ~crc;
