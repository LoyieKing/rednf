// ORIG 0x0822aea6：+0x00 m_amount / +0x04 m_value 均清 0。
// ----------------------------------------------------------------------------
// [b28d-E03 2026-09-10 本波复核] 四数 92/193/2/2 不变（4 件均封顶，无源码旋钮）：
//   · upgrade_table_t::C1 200=200 DIFF：PURE SHIFT c=0（槽直方图全等）⇒ 残差 = 装载序
//     + P30 常量拆分（ORIG `mov idx,%edx; mov this,%eax; lea 0x1c(%edx),%ecx` + disp 0x4
//     vs 我方 `mov idx,%eax; lea 0x1a(%eax),%ecx; mov this,%eax` + disp 0xc，**同址等价**）。
//   · _generateOption 帧 72 vs 56（MIXED）：N82 具名 float 槽序（三形穷举不可达）。
//   · getInvestMaterialInfo 60=60 NEAR：PURE SHIFT c=0（寄存器/装载序）。
//   · get_upgrade_info 371=371 DIFF：PURE SHIFT c=0。
// ----------------------------------------------------------------------------
// [L3-b 2026-09-11 本波核账] 四数 92/193/2/2 不变，落地 1 项**真语义 bug**：
//   · get_upgrade_info 两处折扣缩放：ORIG 0x08900780 段为 `fildl 字段; flds 表系数;
//     fmulp; flds 0x8e122e8; faddp`，0x089007c7 段同形（0x8e122e8 = .rodata 常量池
//     `00 00 00 3f` = **0.5f**）⇒ 源码是 `积 + 0.5f`（fistpll 截断前的就近舍入偏置），
//     旧写法 `-100.0f + 积` 既错常量又错方向（我方 .o 实证发 `flds 100.0f;
//     fsubrp %st,%st(1)`，而 GNU as AT&T 下 `fsubrp %st,%st(1)` ≡ ST(1)−ST(0)，
//     c6root g++4.4.7 探针 `-100.0f + y` 生成同形可证）⇒ 已改 `积 + 0.5f`。
//     归一 diff 136→132 行、our 371 不变（gap 0 保持），四数零变差。
//   · 残差（N 族，穷举记录不变）：C1 我方多 1 条 `_Unwind_Resume`（EH 表族，24 vs 25
//     调用点，指令总数 200=200）+ 7 组 `mov idx,%edx; mov this,%eax; add $K,%edx` vs
//     4.4 `mov idx,%eax; lea K-2(%eax),%ecx; mov this,%eax`（同址等价）；
//     _generateOption/getInvestMaterialInfo 的槽序整体浅/深互换（两序互试已存档）。
// ----------------------------------------------------------------------------
stInvestAmplifyOption_t::stInvestAmplifyOption_t()
{
    m_amount = 0;
    m_value = 0;
}