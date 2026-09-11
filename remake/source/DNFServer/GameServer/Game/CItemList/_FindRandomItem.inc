// [b28d-20] _FindRandomItem（ORIG 0x832d2ce，104/100，nd 48）：残差 =
//   m_resultItems 基址折叠形态（ORIG `add 0x8(%ebp); add $0x4` 两步 vs 4.4
//   lea 合并）+ pair 构造临时槽方向 + 尾块 `mov $0/-0x38` 岛；调用点与谓词
//   一致 ⇒ N86/N93 封顶。
// [b28d-solo-CItemList] **AE（104/104，ndiff 0，p167 11/11 CLEAR）**：
//   ① lowKey/highKey 改匿名临时实参 ⇒ 复现 ORIG 0x0832d318/-0x30、
//   0x0832d38a/-0x20 的 key 槽（具名形落 -0x28/-0x34，N93 槽向）；
//   ② 比较改 `table.end() != lo` ⇒ 复现 ORIG 0x0832d3ff `lea -0x34; mov
//   0x4(%esp)`（operator!=(end,lo) 实参序，从右向左压栈）。旧 NEAR 作废。
int CCompound_ExtreamDun_Item::_FindRandomItem(                     // ORIG T 0x832d2ce
