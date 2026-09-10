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
    register unsigned long k = (key_length >= MT_N) ? key_length : MT_N;

    for (; k != 0; --k)
    {
        m_mt[i] = (m_mt[i] ^
                   ((m_mt[i - 1] ^ (m_mt[i - 1] >> 30)) * 1664525UL)) +
                  init_key[j] + j;
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
        m_mt[i] = (m_mt[i] ^
                   ((m_mt[i - 1] ^ (m_mt[i - 1] >> 30)) * 1566083941UL)) -
                  i;
        ++i;
        if (i >= MT_N)
        {
            m_mt[0] = m_mt[MT_N - 1];
            i = 1;
        }
    }

    m_mt[0] = 0x80000000UL;
    reload();
