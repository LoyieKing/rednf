// ===== Dispatcher_BuyCeraShopItem::useCountDownCoinInFreeCoinDungeon —— ORIG 0x081fe4b8 =====
// ORIG：user!=0；msg+0x36(byte)==1；FindGoods(msg+0x39 int) 非空且 GetItemIdx()==1；
// GetParty() 非空；party+0xcac(CDungeon*) 非空；checkFreeRevivalCondition(dungeon) 非 0 → true。
// [b28c-32] verdict=DIFF：+0x39 具名化+P222 正向包裹；余 N18a 单条 eax→ebx 中转（59=60）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081fe4b8 orig/ours=61/59 ndiff-blocks=2
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x39(%eax),%eax | mov    %eax,%ebx | nop
bool Dispatcher_BuyCeraShopItem::useCountDownCoinInFreeCoinDungeon(
