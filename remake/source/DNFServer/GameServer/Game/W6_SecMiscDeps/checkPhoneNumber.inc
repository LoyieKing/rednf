bool WongWork::CSecurityCardCenter::checkPhoneNumber(char const* phone) const
{
    // [b28d-solo-W6_SecMiscDeps 判词 · N 封顶（N4/P30）] ORIG 0x085fe444：语义逐条
    // 对齐；残余两处（0x085fe456 / 0x085fe460）指针+下标折叠
    // `mov -0x4(%ebp),%eax; add 0xc(%ebp),%eax`（mem 作 add 源）vs 4.4 恒
    // `mov 0xc(%ebp),%edx; lea (%edx,%eax,1),%eax`（b28c-L7 穷举 [] / +i / &arr[i]
    // 三形同产 lea ⇒ 封顶）。
    // ORIG T 0x085fe444 (size 0x52)：i<=0xb 为循环条件；'\0' 提前 break 共享
    // return-1 出口（je→尾块+loop 尾 jmp 对）；数字判定单式
    // 9U < (unsigned)(schar)c - 0x30U（jbe 即无符号序，涵盖 c<'0'）。
    // [P310 翻案] ORIG 0x085fe47f 尾测两段寄存器形（`mov -0x4(%ebp),%eax;
    // cmp $0xb,%eax; setbe`）；旧「unsigned 局部恒折内存直比 ⇒ 封顶」判词作废
    // ⇒ 局部改 int + 显式 `(unsigned int)i` 比较即逐条复现（S108 同法）。
    for (int i = 0; (unsigned int)i <= 0xb; ++i)
    {
        if (phone[i] != '\0')
        {
            if (9U < (unsigned)((int)(signed char)phone[i] - 0x30U))
                return false;
        }
        else
            break;
    }
    return true;
