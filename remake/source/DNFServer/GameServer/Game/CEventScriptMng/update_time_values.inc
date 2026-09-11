// [b28d-solo-CEventScriptMng] update_time_values：AE。
// ORIG 0x0810c5d2：两处修复——① 0x0810c628 `movl $0x0,-0xc(%ebp)` 死局部
// （P232 死槽占帧，令 now/prev/localT/prevT/指针组整体下移 4，全帧槽位与 ORIG
// 全等）；② 0x0810c62f 的 `||` 条件两条守卫（js/jge）同落**出线**的 localTp 块、
// prevTp 块 fall-through（4.1 布局）⇒ 用德摩根正向包裹（P222/P272）条件取反、
// 两块互换复现。p167 CLEAR（4/4），语义闭合。
void CEventScriptMng::update_time_values(int hour)
