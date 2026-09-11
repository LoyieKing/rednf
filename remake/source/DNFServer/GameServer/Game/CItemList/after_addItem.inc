// [b28d-20] after_addItem（ORIG 0x832de18，101 insn / 我方 102，nd 26）：
//   i 改 unsigned 后尾测 `setbe` 与 ORIG 0x832ded9 对齐（原 int 生 setle）；
//   残差 = 循环体首 7 条寄存器角色互换（ORIG edx=i/ecx=j，4.4 ecx=i/eax=j）
//   + P209 附：4.4 对 unsigned 局部尾测恒折 `cmpl $0x3a,mem` 单段（ORIG 为
//   mov+cmp 两段）⇒ N7/N18a 封顶。索引算术已核（48i+12j 与 ORIG 同）。
// [b28d-solo-CItemList] DIFF（ndiff 30↓）| P310：`int i` + 显式
//   `(unsigned int)i < (unsigned int)GROUP_NUM_MAX` ⇒ 复现 ORIG 0x0832ded9
//   `mov -0x14(%ebp),%eax; cmp $0x3a,%eax; setbe` 两段尾测（原 unsigned i
//   被 4.4 折成 `cmpl $0x3a,-0x14` 单段）。T1/T2 直证：mov+cmp 两段 + setbe
//   无符号序。残 = m_bindItems[i][j] 乘法链寄存器角色（ORIG 0x0832de??
//   `lea 0x0(,%eax,4),%ecx`）⇒ N86b。
// [b28d-tu-CItemList 2026-09-09] 复核：101/103（gap +2）——两处 m_bindItems[i][j]
//   索引乘法链的寄存器角色（ORIG `lea 0x0(,%eax,4),%ecx` vs 4.4
//   `lea 0x0(,%edx,4),%eax; mov %eax,%edx`）；P309 全头部声明形实测零变化 ⇒ N86b。
void CCompound_ExtreamDun_Item::after_addItem()                     // ORIG 0x832de18
{
    for (int i = 1; (unsigned int)i < (unsigned int)GROUP_NUM_MAX; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            std::sort(m_data.m_bindItems[i][j].begin(),
                      m_data.m_bindItems[i][j].end());
        }
    }
    for (int k = 0; k < 4; ++k)
    {
        std::sort(m_data.m_resultItems[k].begin(), m_data.m_resultItems[k].end());
    }
}