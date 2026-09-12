// 最小构造/析构（ORIG 逐成员构造属后续精修批次；保证可链接）
// [R18rti 2026-09-13 · B1 已落地，原 N(布局建模) 判词作废] AradItemInfo 由成员改为
// **第二基类**（`class CStackableItem : public CItem, public AradItemInfo`，见头注释）：
// 本件由 DIFF→**AE**（.o size 0x3b1 == ORIG 0x3b1），C1 逐条同形 —— ORIG 0x08515762
// 序 = `call _ZN5CItemC1Ev`(0x85152d4) → `add $0x148; call _ZN12AradItemInfoC1Ev`(0x8515618)
// → 写本类 vptr `movl $0x8c96ea8,(%eax)` → RecipeInfo(+0x174)/LotteryInfo(+0x1c0)/…
// 我方 .o C1（`14:` 起）同序同偏移，仅 vptr 立即数走 `_ZTV14CStackableItem+8` 重定位。
// RTTI：`_ZTI14CStackableItem` 由 si 变 **vmi**（base[1]=AradItemInfo offset_flags 0x14802），
// 并发射 ORIG 实有的 `_ZTI12AradItemInfo`/`_ZTS12AradItemInfo`。
// 消费点 18 处 `m_arad148.` 前缀已全部去除（CStackableItem/*.inc 14 / CItemList/level_check.inc 1 /
// Arad_ItemBonus.cpp 3）；CItem sizeof=0x148 权威 ⇒ 全部偏移/sizeof 不变（末尾 static_assert 兜底）。
CStackableItem::CStackableItem()
{
}