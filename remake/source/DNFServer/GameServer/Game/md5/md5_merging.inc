// ============================================================================
// md5_merging —— ORIG T 0x080b3dbf，size 0xa2（同 TU：md5_finish 0x080b3b8d /
// md5 0x080b3d57 / 本函数 0x080b3dbf / md5_file 0x080b3e61，地址连续）。
//
// 反汇编要点（objdump -d ORIG 0x080b3dbf..0x080b3e61）：
//   ① srcLen clamp：`mov 0xc(%ebp),%eax; cmp $0x400,%eax; jle +2; mov $0x400,%eax;
//      mov %eax,-0x14(%ebp)` ⇒ 具名 int 局部（P0：有槽往返），三目取值。
//   ② memcpy(tmp,src,len)：size/src/dst 依右→左压栈，dst 为立即数 $0x93fa240
//      ⇒ 函数局部 static 缓冲（ORIG 符号 _ZZ11md5_mergingPKciS0_iPhE3tmp @0x093fa240，size 0x400）。
//   ③ 循环：i@-0x10 / j@-0xc 两个具名 int（movl $0,-0x10 / movl $0,-0xc），
//      `jmp` 前置条件 + 尾 `cmp 0x14(%ebp),%eax; setl %al; test %al,%al; jne` ⇒ while(j<dstLen)。
//   ④ `if (i >= len) i = 0;`：`cmp -0x14(%ebp),%eax; jl skip; movl $0,-0x10`。
//   ⑤ `tmp[i] = tmp[i] ^ dst[j]`：i 读两次（LHS 索引 eax / RHS 索引 edx），
//      tmp[i] 与 dst[j] 均为 movzbl 零扩展 ⇒ 两侧都按 unsigned char 语义参与 xor。
//   ⑥ `++i; ++j;`：`addl $0x1,-0x10` / `addl $0x1,-0xc`。
//   ⑦ 尾 `md5(tmp,len,out)`（out→0x8(%esp)、len→0x4(%esp)、tmp→(%esp)），
//      随后直接 `leave; ret`（无 %eax 置位）⇒ 返回 void。
//   帧：push ebp/mov esp,ebp/sub $0x28（无 callee-saved push）。
//   注：ORIG 全库无该符号的调用点（objdump 全 .text `call 0x80b3dbf` 命中 0 处），
//       系独立导出函数；本实现按语义+逐指令转写落位。
// ============================================================================
void md5_merging(const char* src, int srcLen, const char* dst, int dstLen, unsigned char* out)
{
    static unsigned char tmp[0x400];
    // clamp 写作 if 形（非三目）：ORIG 4.1 无 cmov，为
    // `mov 0xc(%ebp),%eax; cmp $0x400,%eax; jle +2; mov $0x400,%eax; mov %eax,-0x14(%ebp)`；
    // 4.4 对三目生 MIN_EXPR cmovle（P230①），改 if 形即出 jle 跳转族。
    int len = srcLen;
    if (len > 0x400)
        len = 0x400;
    memcpy(tmp, src, len);
    int i = 0;
    int j = 0;
    while (j < dstLen)
    {
        if (i >= len)
            i = 0;
        tmp[i] = tmp[i] ^ (unsigned char)dst[j];
        ++i;
        ++j;
    }
    md5(tmp, len, out);
}