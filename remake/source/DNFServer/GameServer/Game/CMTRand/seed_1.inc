// [b28d-L14 N82/算法类] ORIG 0x080d9f5c：init_by_array 通用算法（§6 豁免）；
// ORIG 寄存器指针遍历形，4.4 槽化，gap 129 vs 104。
// [round3-L29 2026-09-10 复核] 算法类（§6）。ORIG 0x080da0?? 用寄存器承载 i/j/k
//   （`mov $0x1,%ebx; mov $0x0,%edi; mov %eax,%esi`）+ `cmp $0x270,%eax; jae` 分支形；
//   已加纯 `register`（P231，非绑定）i/j/k：104→103 条（ORIG 129）；4.4 仍产 cmovae
//   （`-march=i486 -mtune=generic` A/B 实测本 TU 计数不变 ⇒ 路由变更无收益，未上报改路由）。
void CMTRand::seed(unsigned long* init_key, unsigned long key_length)
{
    initialize(19650218UL);

    // [round3-L29] P231：ORIG 以寄存器承载 i/j/k（`mov $0x1,%ebx; mov $0x0,%edi;
    //   mov %eax,%esi`），纯 register 存储建议复现免槽形（非绑定，P0 合规）。
    // [轴 B 2026-09-10 · a_S->U 清单命中件] ORIG 080da007 `cmp $0x26f,%ebx; jle
    //   080da022` 为**有符号**序（%ebx 承载 i）⇒ i 须为 `int`；旧的
    //   `unsigned long i` 使 4.4 产无符号 `jb`（P196 前置已核：ORIG 有符号 ⇒ 允许改）。
    // [AxisB3 属主复核 2026-09-10 ⇒ **保留**] 地址自校验通过：`nm -S` 得
    //   `_ZN7CMTRand4seedEPmm` = [0x080d9f5c, +0x18a)，清单 orig_addr 0x080da00d
    //   落在区间内；我方 .o 该尾测现为 `jle`（原 `jb`）⇒ 符号性已对齐。
    //   实测：our 103→104（gap 26→25，向 ORIG 129 收敛）；TU 四数 3/9/0/3/2
    //   逐值不变 ⇒ 不劣 ⇒ 保留。
    //   §6 算法豁免域（MT19937 init_by_array）标注：改动仅局部变量**类型**
    //   （`unsigned long i`→`int i`），i 值域 [1, MT_N-1] = [1,623]，int 完全覆盖、
    //   无溢出/无符号分叉 ⇒ **算法语义不变、算法正确**；其余 byte 层差异
    //   （ORIG 129 vs 我方 104，寄存器指针遍历形 vs 4.4 槽化 + cmovae）按 §6
    //   「通用算法类只保证算法正确，不强制 identical」封顶，不计入回归。
    register int i = 1;
    register unsigned long j = 0;
    // [L6 #61-75 CMTRand 2026-09-11] ORIG 0x080d9f82-0x080d9f91 为**分支形**：
    //   `mov 0x10(%ebp),%eax; cmp $0x270,%eax; jae +2; mov $0x270,%eax; mov %eax,%esi`
    // 三目会让 4.4 产 `cmpl $0x270,0x10(%ebp); cmovae 0x10(%ebp),%esi`（见 diff）。
    // 按 ORIG 改为「先赋值再 `<` 判断」两句。
    register unsigned long k = key_length;
    if (k >= MT_N)
    {
        // ORIG 形：`cmp $0x270,%eax; jae` 直接跳过赋值（4.1 正向后跳）
    }
    else
    {
        k = MT_N;
    }

    for (; k != 0; --k)
    {
        // [L6 #61-75 CMTRand 2026-09-11] 负结论：ORIG 第一循环把 `m_mt[i]` 副本 spill 到
        //   -0x20(%ebp)（`mov %eax,-0x20(%ebp)` … `mov -0x20(%ebp),%edx`）、把 i 副本
        //   spill 到 -0x24(%ebp) —— 试补 `unsigned long mti = m_mt[i];` 具名局部，
        //   4.4 直接折叠（our 116 不变、ndiff 21→69 劣化）⇒ 回滚。属 4.1 表达式槽化族（N82）。
        m_mt[i] = m_mt[i] ^
                  ((m_mt[i - 1] ^ (m_mt[i - 1] >> 30)) * 1664525UL);
        m_mt[i] += init_key[j] + j;
        // [L6 #61-75 CMTRand 2026-09-11] ORIG 0x080d9fd1-0x080d9fee 为**两条语句**：
        //   ① `mov -0x20(%ebp),%edx; xor %eax,%edx; mov %edx,(%eax,%ecx,4)`
        //      （先落 `m_mt[i] ^ (…)`）② `mov (%eax,%ecx,4),%ecx; lea 0x0(,%edi,4),%eax;
        //      add 0xc(%ebp),%eax; mov (%eax),%eax; add %edi,%eax; add %eax,%ecx;
        //      mov %ecx,(%eax,%edx,4)`（读回后再 `+= init_key[j] + j`）。
        // 单表达式合并会让 4.4 折叠成一次 store（少 2 段 store/reload）⇒ 按 ORIG 拆两句。
        ++i;
        ++j;
        if (i >= MT_N)
        {
            m_mt[0] = m_mt[MT_N - 1];
            i = 1;
        }
        if (j >= key_length)
        {
            j = 0;
        }
    }

    for (k = MT_N - 1; k != 0; --k)
    {
        // [L6 #61-75 CMTRand 2026-09-11] 同第一循环：ORIG 0x080da078-0x080da089 亦为
        // 两条语句（先 `xor` 落 store，再 `mov (%eax,%ecx,4),%ecx; sub %ebx,%ecx` 读回减 i）。
        m_mt[i] = m_mt[i] ^
                  ((m_mt[i - 1] ^ (m_mt[i - 1] >> 30)) * 1566083941UL);
        m_mt[i] -= i;
        ++i;
        if (i >= MT_N)
        {
            m_mt[0] = m_mt[MT_N - 1];
            i = 1;
        }
    }

    m_mt[0] = 0x80000000UL;
    reload();
}