// ============================================================================
// md5_self_test —— ORIG T 0x080b4199，size 0xf8。
//
// 反汇编要点（objdump -d ORIG 0x080b4199..0x080b4291）：
//   ① i@-0xc：`movl $0x0,-0xc` + 入口 jmp 条件 + 尾 `addl $0x1,-0xc` /
//      `cmpl $0x6,-0xc; setle %al; test %al; jne` ⇒ `for (i = 0; i <= 6; ++i)`（P2：
//      setle 直证 `<=`）。
//   ② verbose 段：`cmpl $0x0,0x8(%ebp); je` ⇒ `if (verbose) printf("  MD5 test #%d: ",
//      i + 1);`（格式串立即数 $0x8b1f0a3，.rodata "  MD5 test #%d: "；第二实参先物化）。
//   ③ md5(md5_test_str[i], strlen(md5_test_str[i]), md)（md@-0x1c）：先算串地址
//      （步长 i*81）→ call strlen（结果 %ecx → 0x4(%esp)）→ 压 &md → call md5。
//   ④ memcmp(md, md5_test_sum[i], 0x10)（摘要表步长 i*16）；`test %eax,%eax; je`
//      ⇒ `if (memcmp(...) != 0)`；不等时 `if (verbose) puts("failed");`（串立即数
//      $0x8b1f0b4）→ return 1。
//   ⑤ 相等时 `if (verbose) puts("passed");`（串立即数 $0x8b1f0bb）。
//   ⑥ 循环后 `if (verbose) putchar('\n');`（0xa）→ return 0。
//   帧：sub $0x38。
// ============================================================================
int md5_self_test(int verbose)
{
    int i;
    unsigned char md[16];
    for (i = 0; i <= 6; ++i)
    {
        if (verbose)
            printf("  MD5 test #%d: ", i + 1);
        md5((unsigned char*)md5_test_str[i], (int)strlen(md5_test_str[i]), md);
        if (memcmp(md, md5_test_sum[i], 0x10) != 0)
        {
            if (verbose)
                puts("failed");
            return 1;
        }
        if (verbose)
            puts("passed");
    }
    if (verbose)
        putchar('\n');
    return 0;
}