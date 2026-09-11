// ORIG 0x081bed44 T _ZN18DisPatcher_BuyItem15_onBuyItemEventERK12BuyItemParamP5CUser
// 报告 _onBuyItemEvent.md：push %ebp; mov %esp,%ebp; pop %ebp; ret —— 原始二进制
// 即为纯空实现（购买事件挂钩在 ORIG 中未挂任何回调），忠实转写空函数。
void DisPatcher_BuyItem::_onBuyItemEvent(const BuyItemParam& /*param*/, CUser* /*user*/)

{
}
