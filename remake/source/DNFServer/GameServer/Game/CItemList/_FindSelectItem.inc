// [b28d-20] _FindSelectItem（ORIG 0x832d15e，109/115，nd 85）：残差 = 帧
//   0x48(ORIG，无 push ebx)/0x54(我方，push ebx) + lower_bound/upper_bound 的
//   迭代器临时槽与 this+0x30 绝对偏移寻址寄存器分配；守卫与谓词调用序逐条
//   对应（group==0/>0x3a 双查 + outRarity 有符号窗口）⇒ N7/N82 封顶。
// [b28d-solo-CItemList] DIFF（109/111，ndiff 30↓）| lowKey/highKey 改匿名临时
//   实参 ⇒ 复现 ORIG 0x0832d218/-0x18、0x0832d260/-0x10 key 槽（ndiff 54→30）。
//   残 = m_bindItems[group][outRarity] 二维索引乘法链寄存器角色（ORIG 0x0832d19f
//   `lea 0x0(,%eax,4),%ecx` vs 4.4 eax→edx，多 1 mov×2 处）⇒ N86b；T5 禁改类型
//   （S1 已试 (int)outRarity cast，零变化）。比较用 `lo == tend` 保持。
// [b28d-tu-CItemList 2026-09-09] 复核：109/111（gap +2）——两处
//   m_bindItems[group][outRarity] 索引链寄存器角色（N86b）；lower_bound/
//   upper_bound/BindTable_Predicate 调用点与 ORIG 一致，余为 .o 重定位伪差。
int CCompound_ExtreamDun_Item::_FindSelectItem(                     // ORIG T 0x832d15e
