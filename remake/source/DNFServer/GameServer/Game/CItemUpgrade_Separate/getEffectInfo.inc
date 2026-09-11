// ORIG T 0x08912d10（sret 返回；静态局部 temp 经 __cxa_guard 初始化）。
// [b28c-32] verdict=DIFF：N86/N82（sret 按值返回类对象 + 函数局部 static guard 临时槽跨版本差）。
// [b28d-solo-CItemUpgrade_Separate] 修布局：`if (m_maxLevel < key) return temp;` 早返形
// 复现 ORIG 0x08912d53 的 `jge <find块>` 正极性（旧 `>=` 包裹形 4.4 产 `jl <temp块>` 反向
// 布局）。残余 N86/N82：ORIG 两处独立 temp 拷贝块 vs 4.4 共享尾块（sret/临时槽跨版本差）。
OutLineEFFECTInfoTemp GenuineDamageUpgradeTable::getEffectInfo(int key)
