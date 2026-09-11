// [b28d-L14 N82/算法类] ORIG 0x080cbb90：MT19937 通用算法（§6 算法正确即豁免）；
// ORIG 用寄存器指针遍历，4.4 槽化，gap 59 vs 83。
// [round3-L29 2026-09-10 复核] 算法类（§6 豁免）。ORIG 59 条为**内联 twist + 寄存器指针遍历**
//   （%ebx=this、%esi=kk=0xe3）；我方 81 条（register kk 后 83→81）⇒ N82 槽/寄存器族。
// [P8S5 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
void CMTRand::reload()
{
    // [L6 #61-75 CMTRand 2026-09-11] ORIG 0x080cbb98-0x080cbc2b 为**指针遍历**：
    //   %ebx = p（初值 this，每轮 `add $0x4,%ebx`）、%esi = 递减计数；
    //   实参 `lea 0x4(%ebx),%edx`(p[1]) / `mov %ebx,0x8(%esp)`(*p) /
    //   `lea 0x634(%ebx),%eax`(p[MT_M]，第二循环 `lea -0x38c(%ebx)` = p[MT_M-MT_N])。
    // 索引形（旧）每轮产 lea/shl/add×3 ⇒ 多 22 条（81 vs 59）。按 ORIG 改指针形。
    register unsigned long* p = m_mt;
    // ORIG 0x080cbbc7 尾测为「`test %esi,%esi; setne %al; sub $0x1,%esi; test %al,%al`」
    // ⇒ `while (kk-- != 0)`；第二循环 0x080cbbff 为「`sub $0x1,%esi; test %esi,%esi`」
    // ⇒ `while (--kk != 0)`（递减计数，非 for 递增）。
    register int kk = MT_N - MT_M;
    while (kk-- != 0)
    {
        *p = twist(p[MT_M], *p, p[1]);
        ++p;
    }
    kk = MT_M;
    while (--kk != 0)
    {
        *p = twist(p[MT_M - MT_N], *p, p[1]);
        ++p;
    }
    *p = twist(p[MT_M - MT_N], *p, m_mt[0]);

    m_left = MT_N;
    m_next = m_mt;
}