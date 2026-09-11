// ORIG C1/C2 0x854156a：CData 基类构造后依次默认构造 map/stack/vector 与
// 计时器成员（全部零态）。[P338/㉓ 布局] ORIG 中本定义紧接 NextItemLockTimer
// （0x8541502+0x67=0x854156a）⇒ 保持同一源码顺序，复现 `_GLOBAL__I_` 键符号名
// （IncreaseItemLockTimer）与 .text 发射顺序。
CItemLock::CItemLock()
{
}